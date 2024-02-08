local accTime = 0;
local accTime2 = 0;

local dir = Vector2.new(1.0003, 1)

function Yeastem.Update(deltaTime)
	Object1.Position = Object1.Position + Vector2.new(
		math.cos(accTime2 * 10) * 25.0 * deltaTime, 
		math.sin(accTime2 * 15) * 75.0 * deltaTime
	)

	if not IsKeyDown(1073741906) and not IsKeyDown(" ") then
		Object1.Position.x = Object1.Position.x + math.cos(accTime * 5) * 500.0 * deltaTime;
		Object1.Position.y = Object1.Position.y + math.sin(accTime * 5) * 300.0 * deltaTime;
		accTime = accTime + deltaTime;
	else
		Object1.Position = Object1.Position + deltaTime * (1000 * 1000) * (dir - Vector2.new(1, 1))

		if Object1.Position.x > 700 then
			dir.x = 1 / 1.0003;
		elseif Object1.Position.x < 200 then
			dir.x = 1.0003;
		end
	end

	accTime2 = accTime2 + deltaTime;
end

