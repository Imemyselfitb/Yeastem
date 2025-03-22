print("Hello from Game.lua!")

local function MovePaddles(deltaTime)
	if Keys.IsKeyDown(Keys.UpArrow) and Yeastem.Stem.Paddle2.Position.y - Yeastem.Stem.Paddle2.height / 2 > 0 then
		Yeastem.Stem.Paddle2.Position.y = Yeastem.Stem.Paddle2.Position.y - deltaTime * 500
	end
	if Keys.IsKeyDown(Keys.DownArrow) and Yeastem.Stem.Paddle2.Position.y + Yeastem.Stem.Paddle2.height / 2 < Window.height then
		Yeastem.Stem.Paddle2.Position.y = Yeastem.Stem.Paddle2.Position.y + deltaTime * 500
	end
	if Keys.IsKeyDown("W") and Yeastem.Stem.Paddle1.Position.y - Yeastem.Stem.Paddle1.height / 2 > 0 then
		Yeastem.Stem.Paddle1.Position.y = Yeastem.Stem.Paddle1.Position.y - deltaTime * 500
	end
	if Keys.IsKeyDown("S") and Yeastem.Stem.Paddle1.Position.y + Yeastem.Stem.Paddle1.height / 2 < Window.height then
		Yeastem.Stem.Paddle1.Position.y = Yeastem.Stem.Paddle1.Position.y + deltaTime * 500
	end

	Yeastem.Stem.Paddle1.Position.x = 150;
	Yeastem.Stem.Paddle2.Position.x = Window.width - 150;
end

local speed = 350
local vel = Vector2.new(1, 0)
local CheckCollisionBetweenRectangles = nil

local LeftScore = 0
local RightScore = 0

local function ResetGame()
	Yeastem.Stem.Paddle1.Position = Vector2.new(Yeastem.Stem.Paddle1.StartPosition)
	Yeastem.Stem.Paddle2.Position = Vector2.new(Yeastem.Stem.Paddle2.StartPosition)

	Yeastem.Stem.Ball.Position.x = Window.width / 2;
	Yeastem.Stem.Ball.Position.y = Window.height / 2;

	speed = 350
	vel = Vector2.new(1, 0)

	local extraInfo = ""
	if LeftScore ~= RightScore then	
		extraInfo = " to " .. ((LeftScore > RightScore) and "LEFT" or "RIGHT")
	end

	local text = "Score: " .. tostring(LeftScore) .. " - " .. tostring(RightScore) .. extraInfo
	
	-- panels.lua
	UpdatePanels(text)
end

local function MoveBall(deltaTime)
	Yeastem.Stem.Ball.Position = Yeastem.Stem.Ball.Position + vel * deltaTime * speed
	Yeastem.Stem.Ball.dir = math.atan(vel.y, vel.x) * 180 / 3.141592

	if Yeastem.Stem.Ball.Position.y - Yeastem.Stem.Ball.height / 2 < 0 then
		vel.y = 0 - vel.y
		Yeastem.Stem.Ball.Position = Yeastem.Stem.Ball.Position + vel * deltaTime * speed
	elseif Yeastem.Stem.Ball.Position.y + Yeastem.Stem.Ball.height / 2 > Window.height then
		vel.y = 0 - vel.y
		Yeastem.Stem.Ball.Position = Yeastem.Stem.Ball.Position + vel * deltaTime * speed
	end

	if Yeastem.Stem.Ball.Position.x - Yeastem.Stem.Ball.width / 2 < 0 then
		print("RIGHT Scores!")
		RightScore = RightScore + 1
		ResetGame()
	elseif Yeastem.Stem.Ball.Position.x + Yeastem.Stem.Ball.width / 2 > Window.width then
		print("LEFT Scores!")
		LeftScore = LeftScore + 1
		ResetGame()
	end
end

local function CheckCollision()
	if CheckCollisionBetweenRectangles(Yeastem.Stem.Ball, Yeastem.Stem.Paddle1) then
		vel.x = 1
		vel.y = 0.5 * vel.y + 0.5 * (Yeastem.Stem.Ball.Position.y - Yeastem.Stem.Paddle1.Position.y) / Yeastem.Stem.Paddle1.height
		speed = speed + 5
	end

	if CheckCollisionBetweenRectangles(Yeastem.Stem.Ball, Yeastem.Stem.Paddle2) then
		vel.x = -1
		vel.y = (Yeastem.Stem.Ball.Position.y - Yeastem.Stem.Paddle2.Position.y) / Yeastem.Stem.Paddle2.height
		speed = speed + 5
	end
end

local accTime = 0

function Update(deltaTime)
	if not Yeastem.Stem.Paddle1.StartPosition then
		CheckCollisionBetweenRectangles = Yeastem.Stem.RectCollisionModule.CheckCollisionBetweenRectangles
		Yeastem.Stem.Paddle1.StartPosition = Vector2.new(Yeastem.Stem.Paddle1.Position)
		Yeastem.Stem.Paddle2.StartPosition = Vector2.new(Yeastem.Stem.Paddle2.Position)
		UpdatePanels = Yeastem.Stem.Panels.UpdatePanels
		print("START!")
	end

	paddleSpeed1 = 0.0004 * 100
	paddleSpeed2 = 0.0004 * 100

	if math.abs(Yeastem.Stem.Ball.Position.y - Yeastem.Stem.Paddle1.Position.y) > Yeastem.Stem.Paddle1.height / 2 then
		Yeastem.Stem.Paddle1.Position.y = Yeastem.Stem.Paddle1.Position.y + paddleSpeed1 * (Yeastem.Stem.Ball.Position.y - Yeastem.Stem.Paddle1.Position.y)
	end

	if math.abs(Yeastem.Stem.Ball.Position.y - Yeastem.Stem.Paddle2.Position.y) > Yeastem.Stem.Paddle1.height / 2 then
		Yeastem.Stem.Paddle2.Position.y = Yeastem.Stem.Paddle2.Position.y + paddleSpeed2 * (Yeastem.Stem.Ball.Position.y - Yeastem.Stem.Paddle2.Position.y)
	end

	MovePaddles(deltaTime)
	CheckCollision()
	MoveBall(deltaTime)

	Yeastem.Stem.Paddle2.Scale = Yeastem.Stem.Paddle2.Scale + math.sin(accTime * 20) * deltaTime * 1;
	accTime = accTime + deltaTime;
end
