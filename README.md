# loveslang

A library providing a Slang compiler interface for LÖVE projects.

# Compilation

This project uses cmake. I personally prefer to use this one-liner:

```bash
( cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 && cd build/ && cmake --build . -j4 ) && love tests/main.lua
```


# Basic usage

```lua
local loveslang = assert(package.loadlib("path/to/libloveslang.so", "luaopen_loveslang"))()
local compiler = loveslang.newCompiler({
    paths = {"shadersrc"},
})

local glsl, reflection = compiler:compileToGLSL("my-shader") -- Will look for `shadersrc/my-shader.slang`
```

# Limitations

- Currently, the entrypoints `vertexMain`, `pixelMain`, and `computeMain` are hardcoded.
- The reflection format is likely to undergo breaking changes.
- Variable mappings are not currently generated for scalar uniforms (e.g. float4, boolean)
- Slang compiler is dynamically linked
- Completely untested outside Linux
