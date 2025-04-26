print("Hello from Rect... .lua!")

function CheckCollisionBetweenRectangles(ObjectA, ObjectB)
	local left1 = ObjectA.position.x - ObjectA.width / 2
	local right1 = ObjectA.position.x + ObjectA.width / 2
	local top1 = ObjectA.position.y - ObjectA.height / 2
	local bottom1 = ObjectA.position.y + ObjectA.height / 2

	local left2 = ObjectB.position.x - ObjectB.width / 2
	local right2 = ObjectB.position.x + ObjectB.width / 2
	local top2 = ObjectB.position.y - ObjectB.height / 2
	local bottom2 = ObjectB.position.y + ObjectB.height / 2

	return not (left2 > right1 or right2 < left1 or top2 > bottom1 or bottom2 < top1);
end
