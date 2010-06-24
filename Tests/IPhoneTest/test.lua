require("LuaGame")

print("Testing testing lua integration!")


function LuaGame:Init()
	print("Time to INIT some stuff!")
end

function LuaGame:PreLoad()
	print("Time to PRELOAD some data!")
end

function LuaGame:Update(dt)
	print("Time to UPDATE our game with '" .. tostring(dt) .. "'")
end

function LuaGame:Render()
	print("Time to RENDER our game!")
end