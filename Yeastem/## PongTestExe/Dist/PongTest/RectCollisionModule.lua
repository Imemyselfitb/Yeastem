function CheckCollisionBetweenRectangles(ObjectA, ObjectB)
	local left1 = ObjectA.Position.x - ObjectA.width / 2
	local right1 = ObjectA.Position.x + ObjectA.width / 2
	local top1 = ObjectA.Position.y - ObjectA.height / 2
	local bottom1 = ObjectA.Position.y + ObjectA.height / 2

	local left2 = ObjectB.Position.x - ObjectB.width / 2
	local right2 = ObjectB.Position.x + ObjectB.width / 2
	local top2 = ObjectB.Position.y - ObjectB.height / 2
	local bottom2 = ObjectB.Position.y + ObjectB.height / 2

	--print(left2 > right1, right2 < left1, top2 > bottom1, bottom2 < top1)

	return not (left2 > right1 or right2 < left1 or top2 > bottom1 or bottom2 < top1);
end
