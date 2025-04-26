print("Hello from Game.lua!")

Ball = Yeastem.script.parent.Ball
Paddle1 = Yeastem.script.parent.Paddle1
Paddle2 = Yeastem.script.parent.Paddle2

local function MovePaddles(deltaTime)
	if Keys.IsKeyDown(Keys.UpArrow) and Paddle2.position.y - Paddle2.height / 2 > 0 then
		Paddle2.position.y = Paddle2.position.y - deltaTime * 500
	end
	if Keys.IsKeyDown(Keys.DownArrow) and Paddle2.position.y + Paddle2.height / 2 < Window.height then
		Paddle2.position.y = Paddle2.position.y + deltaTime * 500
	end
	if Keys.IsKeyDown("W") and Paddle1.position.y - Paddle1.height / 2 > 0 then
		Paddle1.position.y = Paddle1.position.y - deltaTime * 500
	end
	if Keys.IsKeyDown("S") and Paddle1.position.y + Paddle1.height / 2 < Window.height then
		Paddle1.position.y = Paddle1.position.y + deltaTime * 500
	end

	Paddle1.position.x = 150;
	Paddle2.position.x = Window.width - 150;
end

local speed = 350
local vel = Vector2.new(1, 0)
local CheckCollisionBetweenRectangles = nil

local leftScore = 0
local rightScore = 0

local function ResetGame()
	Paddle1.position = Vector2.new(Paddle1.Startposition)
	Paddle2.position = Vector2.new(Paddle2.Startposition)

	Ball.position.x = Window.width / 2;
	Ball.position.y = Window.height / 2;

	speed = 350
	vel = Vector2.new(1, 0)

	local extraInfo = ""
	if leftScore ~= rightScore then	
		extraInfo = " to " .. ((leftScore > rightScore) and "LEFT" or "RIGHT")
	end

	local text = "Score: " .. tostring(leftScore) .. " - " .. tostring(rightScore) .. extraInfo
	
	-- panels.lua
	UpdatePanels(text)
end

local function MoveBall(delta)
	Ball.position = Ball.position + vel * delta * speed
	Ball.dir = math.atan(vel.y, vel.x) * 180 / 3.141592

	if Ball.position.y - Ball.height / 2 < 0 then
		vel.y = 0 - vel.y
		Ball.position = Ball.position + vel * delta * speed
	elseif Ball.position.y + Ball.height / 2 > Window.height then
		vel.y = 0 - vel.y
		Ball.position = Ball.position + vel * delta * speed
	end

	if Ball.position.x - Ball.width / 2 < 0 then
		print("RIGHT Scores!")
		rightScore = rightScore + 1
		ResetGame()
	elseif Ball.position.x + Ball.width / 2 > Window.width then
		print("LEFT Scores!")
		leftScore = leftScore + 1
		ResetGame()
	end
end

local function CheckCollision()
	if CheckCollisionBetweenRectangles(Ball, Paddle1) then
		vel.x = 1
		vel.y = 0.5 * vel.y + 0.5 * (Ball.position.y - Paddle1.position.y) / Paddle1.height
		speed = speed + 5
	end

	if CheckCollisionBetweenRectangles(Ball, Paddle2) then
		vel.x = -1
		vel.y = (Ball.position.y - Paddle2.position.y) / Paddle2.height
		speed = speed + 5
	end
end

local accTime = 0

function Update(delta)
	if not Paddle1.Startposition then
		CheckCollisionBetweenRectangles = Yeastem.script.parent.RectCollisionModule.CheckCollisionBetweenRectangles
		Paddle1.Startposition = Vector2.new(Paddle1.position)
		Paddle2.Startposition = Vector2.new(Paddle2.position)
		UpdatePanels = Yeastem.script.parent.Panels.UpdatePanels
		print("START!")
	end

	paddleSpeed1 = 0.0004 * 1
	paddleSpeed2 = 0.0004 * 1

	if math.abs(Ball.position.y - Paddle1.position.y) > Paddle1.height / 2 then
		Paddle1.position.y = Paddle1.position.y + paddleSpeed1 * (Ball.position.y - Paddle1.position.y)
	end

	if math.abs(Ball.position.y - Paddle2.position.y) > Paddle1.height / 2 then
		Paddle2.position.y = Paddle2.position.y + paddleSpeed2 * (Ball.position.y - Paddle2.position.y)
	end

	MovePaddles(delta)
	CheckCollision()
	MoveBall(delta)

	--Paddle1.scale = Paddle1.scale + math.sin(accTime * 20) * delta * math.max(rightScore - leftScore, 0);
	--Paddle2.scale = Paddle2.scale + math.sin(accTime * 20) * delta * math.max(leftScore - rightScore, 0);
	accTime = accTime + delta;
end
