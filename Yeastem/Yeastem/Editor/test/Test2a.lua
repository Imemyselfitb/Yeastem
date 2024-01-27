local accTime = 0;

function Update(deltaTime)
	Object1.x = Object1.x + math.cos(accTime * 5) * 500.0 * deltaTime;
	Object1.y = Object1.y + math.sin(accTime * 5) * 300.0 * deltaTime;

	accTime = accTime + deltaTime;
end

