print("test lua")


rl = require("rl")

rl.InitWindow(800, 600, "Raylib LuaJIT Shapes Test")
rl.SetTargetFPS(60)

-- local angle = 0

while not rl.WindowShouldClose() do
    rl.BeginDrawing()
    rl.ClearBackground(rl.RAYWHITE)

--     -- Draw shapes with existing functions
--     rl.DrawPixel(50, 50, rl.RED)
--     rl.DrawLine(100, 100, 200, 200, rl.BLUE)
--     rl.DrawCircle(150, 150, 50, rl.GREEN)
--     rl.DrawRectangle(250, 100, 100, 80, rl.PURPLE)
--     rl.DrawRectangleRec({350, 100, 100, 80}, rl.YELLOW)

--     -- Draw new shapes
--     rl.DrawCircleGradient(150, 300, 60, rl.RED, rl.BLUE)
--     rl.DrawCircleLines(150, 300, 70, rl.BLACK)

--     rl.DrawRectangleLines(250, 250, 100, 80, rl.BLACK)
--     rl.DrawRectangleGradientV(250, 350, 100, 80, rl.GREEN, rl.YELLOW)

--     rl.DrawTriangle({400, 200}, {450, 300}, {350, 300}, rl.RED)
--     rl.DrawTriangleLines({400, 350}, {450, 450}, {350, 450}, rl.BLUE)

--     -- Rotating polygon
--     angle = angle + 1
--     rl.DrawPoly({600, 300}, 6, 50, angle, rl.PURPLE)

--     -- Instructions
--     rl.DrawText("Testing rshapes module", 10, 10, 20, rl.BLACK)
--     rl.DrawText("Press Q to quit", 10, 30, 20, rl.GRAY)

    rl.EndDrawing()

    if rl.IsKeyPressed(rl.KEY_Q) then
        break
    end
end

rl.CloseWindow()