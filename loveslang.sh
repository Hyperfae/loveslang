#!/usr/bin/env bash
input="$1"
set -euo pipefail
shift

applyCommonFixups() {
    cat \
    | grep -v "^#version ...$" \
    | grep -v "^layout(row_major) buffer;$" \
    | grep -v "^layout(binding = .)$" \
    | sed -z "s/layout(std140) uniform block_GlobalParams_0.*globalParams_0;/uniform GlobalParams_0 globalParams_0;/" \
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
            | sed "s/void main()/void pixelmain()/"
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
