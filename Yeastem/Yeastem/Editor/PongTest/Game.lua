local function MovePaddles(deltaTime)
	if Keys.IsKeyDown(Keys.UpArrow) and Object2.Position.y - Object2.height / 2 > 0 then
		Object2.Position.y = Object2.Position.y - deltaTime * 500
	end
	if Keys.IsKeyDown(Keys.DownArrow) and Object2.Position.y + Object2.height / 2 < Window.height then
		Object2.Position.y = Object2.Position.y + deltaTime * 500
	end
	if Keys.IsKeyDown("W") and Object1.Position.y - Object1.height / 2 > 0 then
		Object1.Position.y = Object1.Position.y - deltaTime * 500
	end
	if Keys.IsKeyDown("S") and Object1.Position.y + Object1.height / 2 < Window.height then
		Object1.Position.y = Object1.Position.y + deltaTime * 500
	end
end

local speed = 350
local vel = Vector2.new(1, 0)

local LeftScore = 0
local RightScore = 0

local function ResetGame()
	Object1.Position = Vector2.new(Object1.StartPosition)
	Object2.Position = Vector2.new(Object2.StartPosition)
	Object3.Position = Vector2.new(Object3.StartPosition)

	speed = 350
	vel = Vector2.new(1, 0)

	local extraInfo = ""
	if LeftScore ~= RightScore then	
		extraInfo = " to " .. ((LeftScore > RightScore) and "LEFT" or "RIGHT")
	end

	local text = "Score: " .. tostring(LeftScore) .. " - " .. tostring(RightScore) .. extraInfo
	Update(text)
end

local function MoveBall(deltaTime)
	Object3.Position = Object3.Position + vel * deltaTime * speed

	if Object3.Position.y - Object3.height / 2 < 0 then
		vel.y = 0 - vel.y
		Object3.Position = Object3.Position + vel * deltaTime * speed
	elseif Object3.Position.y + Object3.height / 2 > Window.height then
		vel.y = 0 - vel.y
		Object3.Position = Object3.Position + vel * deltaTime * speed
	end

	if Object3.Position.x - Object3.width / 2 < 0 then
		print("RIGHT Scores!")
		RightScore = RightScore + 1
		ResetGame()
	elseif Object3.Position.x + Object3.width / 2 > Window.width then
		print("LEFT Scores!")
		LeftScore = LeftScore + 1
		ResetGame()
	end
end

local function CheckCollision()
	if CheckCollisionBetweenRectangles(Object3, Object1) then
		vel.x = 1
		speed = speed + 5
		vel.y = 0.5 * vel.y + 0.5 * (Object3.Position.y - Object1.Position.y) / Object1.height
	end

	if CheckCollisionBetweenRectangles(Object3, Object2) then
		vel.x = -1
		speed = speed + 5
		vel.y = (Object3.Position.y - Object2.Position.y) / Object2.height
	end
end

function Yeastem.Update(deltaTime)
	if not Object1.StartPosition then
		Object1.StartPosition = Vector2.new(Object1.Position)
		Object2.StartPosition = Vector2.new(Object2.Position)
		Object3.StartPosition = Vector2.new(Object3.Position)

		print("START!")
	end

	if math.abs(Object3.Position.y - Object1.Position.y) > Object1.height / 2 then
		Object1.Position.y = Object1.Position.y + 0.0004 * (Object3.Position.y - Object1.Position.y)
	end

	if math.abs(Object3.Position.y - Object2.Position.y) > Object1.height / 2 then
		Object2.Position.y = Object2.Position.y + 0.0004 * (Object3.Position.y - Object2.Position.y)
	end

	MovePaddles(deltaTime)
	CheckCollision()
	MoveBall(deltaTime)
end
