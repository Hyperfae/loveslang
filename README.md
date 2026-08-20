# loveslang
A slang library (and compiler/post-processor program) to allow writing shaders
targetting LÖVE's flavor of GLSL. Currently, the post-processor exists as a Bash
script, but the plan is to eventually rewrite it as a C++ program utilizing
Slang's compilation API. Maybe this could also allow for creating nice-to-use,
type-safe Lua bindings for shader paramaters?

# Basic usage

```Bash
./loveslang.sh ./test.slang PIXEL=mainPixel > myPixelShader.glsl
```

Future plan is to automatically infer entrypoint function names using Slang's reflection API.
