---@class testcommon
local testcommon = {}

function testcommon.createTestImage()
    local canvas = love.graphics.newCanvas(320, 240)
    canvas:setFilter("nearest")
    love.graphics.setCanvas(canvas)
    love.graphics.clear(1,1,1,1)
    love.graphics.setCanvas()
    return love.graphics.newImage(love.graphics.readbackTexture(canvas))
end

function testcommon.drawshader(shader, reflection)
    local canvas = testcommon.createTestImage()
    local time = 0.0
    function love.draw()
        love.graphics.setShader(shader)
        if shader:hasUniform(reflection.time.name) then
            shader:send(reflection.time.name, time)
        end
        local sw, sh = love.graphics.getDimensions()
        love.graphics.draw(canvas, 0, 0, 0, sw / canvas:getWidth(), sh / canvas:getHeight())
    end
    function love.update(dt)
        time = time + dt/4
        time = time % 1
    end
end

return testcommon
