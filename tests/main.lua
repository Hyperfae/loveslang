jit.off()
-- function love.load()
    local loveslang = assert(package.loadlib("./build/libloveslang.so", "luaopen_loveslang"))()
    local testcommon = require("common") ---@as testcommon

    print(love.timer.getTime() .. " - Before first compiler")
    print(tostring(loveslang.newCompiler()) .. " <-- compiler")
    print(love.timer.getTime() .. " - Before second compiler")
    local compiler = loveslang.newCompiler({
        paths = {"lib"}
    })
    print(love.timer.getTime() .. " - after second compiler")

    local source, reflection = compiler:compileToGLSL("test")
    -- print(source)
    print(love.timer.getTime() .. " - after compilation")

    love.graphics.newComputeShader(source)
    local shader = love.graphics.newShader(source)

    testcommon.drawshader(shader)
-- end