---@class testcommon
local testcommon = {}

function testcommon.drawshader(shader)
    local canvas = love.graphics.newCanvas(320, 240)
    love.graphics.setCanvas(canvas)
    love.graphics.clear(1,1,1,1)
    love.graphics.setCanvas()
    function love.draw()
        love.graphics.setShader(shader)
        love.graphics.draw(canvas)
    end
end

return testcommon
