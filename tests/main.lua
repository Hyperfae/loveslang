jit.off()
print(love.timer.getTime() .. " - Before lib load")
local loveslang = assert(package.loadlib("./build/libloveslang.so", "luaopen_loveslang"))()
local testcommon = require("common") ---@as testcommon

print(love.timer.getTime() .. " - Before first compiler")
print(tostring(loveslang.newCompiler()) .. " <-- compiler")
print(love.timer.getTime() .. " - Before second compiler")
local compiler = loveslang.newCompiler({
    paths = {"lib"}
})
print(love.timer.getTime() .. " - after second compiler")

local source, reflection = compiler:compileToGLSL("test.slang")

local shader = love.graphics.newShader(source)

-- local shader, uniformMap = compiler:newShader("test.slang")

testcommon.drawshader(shader)
