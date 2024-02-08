local accTime = 0;

function Yeastem.Update(deltaTime)
	Object2.scale = Object2.scale + math.sin(accTime * 5) * 1.0 * deltaTime;
	Object2.dir = Object2.dir + deltaTime;

	accTime = accTime + deltaTime;
end

