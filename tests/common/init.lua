---@class testcommon
local testcommon = {}

function testcommon.drawshader(shader, reflection)
    local canvas = love.graphics.newCanvas(320, 240)
    canvas:setFilter("nearest")
    love.graphics.setCanvas(canvas)
    love.graphics.clear(1,1,1,1)
    love.graphics.setCanvas()
    local time = 0.0
    function love.draw()
        love.graphics.setShader(shader)
        if shader:hasUniform("globalParams_0.time_0") then
            shader:send("globalParams_0.time_0", time)
        end
        local sw, sh = love.graphics.getDimensions()
        love.graphics.draw(canvas, 0, 0, 0, sw / canvas:getWidth(), sh / canvas:getHeight())
    end
    function love.update(dt)
        time = time + dt
        time = time % 1
    end
end

return testcommon
