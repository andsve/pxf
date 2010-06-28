
function luagame:Init()
	self.GameIdent = "Our awesome testgame!"
	self.GameVersion = "1.0"
	
	testvec = luagame.vec2.new()
	print("testvec: " .. testvec:tostring())
	print("testvec.instance: " .. tostring(testvec.instance))
	
	testvec.instance = 123
	print("Should fail now: " .. testvec:tostring())
	
	--print("luagame.vec2.ToString(): " .. luagame.vec2.ToString())
end

function luagame:PreLoad()
	print("Time to PRELOAD some data!")
	-- TODO: Does not work, at all.
	--       Should add resources that need to be loaded. LuaGame should then
	--       iterate over them and load them into a resource/data pool of some sort.
end

function luagame:Update(dt)
	--print("Time to UPDATE our game with '" .. tostring(dt) .. "'")
end

function luagame:Render()
	--print("Time to RENDER our game!")
end