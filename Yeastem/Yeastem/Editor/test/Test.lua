local accTime = 0;

function Update(deltaTime)
	Object1.x = Object1.x + math.cos(accTime * 5) * 500.0 * deltaTime;
	Object1.y = Object1.y + math.sin(accTime * 5) * 300.0 * deltaTime;

	Object2.scale = Object2.scale + math.sin(accTime * 5) * 1.0 * deltaTime;
	Object2.dir = Object2.dir + deltaTime;

	accTime = accTime + deltaTime;
end

