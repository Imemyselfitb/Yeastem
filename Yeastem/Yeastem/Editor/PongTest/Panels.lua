local p = Panel.new("Pong!")
local q = Panel.createElement("Button", "Score: 0 - 0")

p:AppendChild(q);
p:Submit()

q.OnClick = (function()
	q.Name = q.Name .. "."
end)

function Update(text) -- To show if a function is notated without the "Yeastem." it could be called in code
	q.Name = text
end
