
function LuaGame:Init()
	self.GameIdent = "Our awesome testgame!"
	self.GameVersion = "1.0"
	
	print("luagame.vec2.ToString(): " .. LuaGame.vec2.ToString())
end

function LuaGame:PreLoad()
	print("Time to PRELOAD some data!")
	-- TODO: Does not work, at all.
	--       Should add resources that need to be loaded. LuaGame should then
	--       iterate over them and load them into a resource/data pool of some sort.
end

function LuaGame:Update(dt)
	--print("Time to UPDATE our game with '" .. tostring(dt) .. "'")
end

function LuaGame:Render()
	--print("Time to RENDER our game!")
end