module("LuaGame")

LuaGame = {}
LuaGame.ass = "balls"

function LuaGame:poop()
	print("Poop: " .. tostring(self.ass))
end