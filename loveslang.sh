#!/usr/bin/env bash
input="$1"
set -euo pipefail
shift

applyCommonFixups() {
    cat \
    | grep -v "^#version ...$" \
    | grep -v "^layout(row_major) buffer;$" \
    | grep -v "^layout(binding = .)$" \
    $(: "Convert globalparams block to default block struct" ) \
    | sed -z "s/layout(std140) uniform block_GlobalParams_0.*globalParams_0;/uniform GlobalParams_0 globalParams_0;/" \
    $(: "Fixup RWStructuredBuffer names" ) \
    $(: "This is kind of a nightmare regex, so it'll need some serious commentating." ) \
    | sed -Ez "s/$(: \
            Match the first line. \
        )buffer (\w+) \{[\n\s]*$(: \
            Match the type qualifier of the buffer. Sed does not support "\w" inside "[]" so we have to do this. \
        ) +(\w+) +([a-zA-Z0-9_]+\[\]);$(: \
            Match the ending \
        )[\n\s]*\} (\w+);$(: \
            Final output format \
        )/buffer in_\4 { \2 \3; } \4;/" \
    | cat
}

while [ "$#" -gt 0 ]; do
    if [[ "$1" = "VERTEX="* ]]; then
        entrypoint="${1#"VERTEX="}"
        echo "#ifdef VERTEX"
        slangc -DVARYING=out -target glsl "$input" -entry "$entrypoint" \
            | applyCommonFixups \
            | sed "s/void main()/void vertexmain()/"
        echo "#endif"
    elif [[ "$1" = "PIXEL="* ]]; then
        entrypoint="${1#"PIXEL="}"
        echo "#ifdef PIXEL"
        slangc -DVARYING=in -target glsl "$input" -entry "$entrypoint" \
            | applyCommonFixups \
            | sed "s/void main()/void effect()/"
        echo "#endif"
    elif [[ "$1" = "COMPUTE="* ]]; then
        entrypoint="${1#"COMPUTE="}"
        echo "#ifdef COMPUTE"
        slangc -DVARYING=in -target glsl "$input" -entry "$entrypoint" \
            | applyCommonFixups \
            | sed "s/void main()/void computemain()/"
        echo "#endif"
    fi
    shift
done
