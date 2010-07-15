
require("faker")

function luagame:Init()
	self.GameIdent = "Our awesome testgame!"
	self.GameVersion = "1.0"
	
	luagame:add_console("GameIdent: ^4" .. self.GameIdent)
	luagame:add_console("GameVersion: ^4" .. self.GameVersion)
	
	
	--testvec = luagame.vec2.new()
	--print("testvec: " .. testvec:tostring())
	--print("testvec.instance: " .. tostring(testvec.instance))
	
	---------------------------------------------------------
	-- Fail guard test for "class" member calling.
	--testvec.instance = 123
	--print("Should fail now: " .. testvec:tostring())
	---------------------------------------------------------
	
	--print("luagame.vec2.ToString(): " .. luagame.vec2.ToString())
	
	screenw, screenh = luagame.graphics.getscreensize()
	luagame:add_console("Screen size: ^4" .. tostring(screenw) .. "x" .. tostring(screenh))
	
	sprite_test = luagame.graphics:newsprite(sprite_texture,64,64,10)

	testani = 0.0
end

function luagame:PreLoad()
	test_texture = luagame.resources.loadtexture("test_atlas.png")
	test_texture2 = luagame.resources.loadtexture("test.png")
	sprite_texture = luagame.resources.loadtexture("sprite_test.jpg")

	-- preload alot of unnessesary textures to try loadingbar
	test_texture3 = luagame.resources.loadtexture("test_atlas.png")
	test_texture4 = luagame.resources.loadtexture("test_atlas.png")
	test_texture5 = luagame.resources.loadtexture("test_atlas.png")
	test_texture6 = luagame.resources.loadtexture("test_atlas.png")
	
	--luagame.iphone.gettext("LuaGame", "Console input", "Command")
end

function luagame:Update(dt)
	--print("Time to UPDATE our game with '" .. tostring(dt) .. "'")
	testani = testani + dt*0.3
	--mx,my = luagame.mouse.getmousepos()
	--luagame:add_console("Mouse x: " .. tostring(mx) .. ", y: " .. tostring(my))
end

function luagame:Render()
	--print("Time to RENDER our game!")
	
	--[[luagame.graphics.drawquad(100+math.cos(testani)*10, 100+math.sin(testani)*10, 20, 20)
	luagame.graphics.drawquad(50+math.cos(testani+23)*10, 100+math.sin(testani+23)*10, 20, 20)
	luagame.graphics.drawquad(100+math.cos(testani+45)*10, 50+math.sin(testani+12)*10, 20, 20)
	
	luagame.graphics.rotate((2.0*3.14) / 123.0)
	luagame.graphics.drawquad(100, 200, 30, 30)
	--luagame.graphics.rotate(0.0)
	luagame.graphics.translate(math.cos(testani)*20, math.sin(testani)*10)
	luagame.graphics.drawquad(100, 200, 30, 30)
	
	for i=1,100 do
	 luagame.graphics.drawquad(100, 200+(i*3), 100, 2)
	end
	--luagame.graphics.drawquad(10, 10, 40, 40)
	
	luagame.graphics.translate(screenw / 2.0, screenh / 2.0)
	luagame.graphics.drawquad(0, 0, 30, 30)
	--luagame.graphics.translate(math.cos(testani)*20, math.sin(testani)*10)
	luagame.graphics.translate(50, 0)
	luagame.graphics.drawquad(0, 0, 30, 30)
	
	luagame.graphics.translate(-50, 0)
	luagame.graphics.translate(-50, 0)
	luagame.graphics.rotate(testani)
	luagame.graphics.drawquad(0, 0, 30, 30)
	--]]
	
	--luagame.graphics.translate(screenw / 2.0, screenh / 2.0)
	--luagame.graphics.drawquad(0, 0, 30, 30)
	
	--[[
	test_texture:bind()
	
	-- reset coord system
	luagame.graphics.loadidentity()
	luagame.graphics.rotate(testani)
	luagame.graphics.translate(screenw / 2.0, screenh / 2.0)
	luagame.graphics.drawquad(0, 0, 512, 512)
	
	-- reset coord system
	luagame.graphics.loadidentity()
	luagame.graphics.translate(screenw / 2.0, screenh / 2.0)
	luagame.graphics.drawquad(50, 0, 30, 30, testani / 2.0)
	
	luagame.graphics.rotate(testani)
	luagame.graphics.drawquad(0, 0, 30, 30)
	
	luagame.graphics.translate(50.0, 0.0)
	luagame.graphics.rotate(testani)
	luagame.graphics.drawquad(0, 0, 30, 30)
	]]
	
	a,b = test_texture2:getsize()
	luagame:add_console("texture size: " .. tostring(a) .. "x" .. tostring(b))
	
	-- test draw texture 2
	test_texture2:bind()
	
	luagame.graphics.loadidentity()
	luagame.graphics.translate(screenw / 2.0, screenh / 2.0)
	luagame.graphics.drawquad(-64, -64, 256, 128)
	
	-- back to texture 1
	test_texture:bind()
	luagame.graphics.loadidentity()
	luagame.graphics.translate(screenw / 2.0, screenh / 2.0)
	luagame.graphics.drawquad(0, 0, 32, 32, 0, 0, 16, 16)
	
	-- render debug text
	luagame.graphics.loadidentity()
	luagame:draw_console()
	
	--luagame:add_console("Rendering frame: ^4" .. tostring(simple_framecount))
	
	-- make it crash:
  --luagame.graphics.drawquad(nil)
end


function luagame:TextInput(str)
  luagame:add_console("> " .. str)
  ret = runstring("return " .. str)
  luagame:add_console("^4" .. tostring(ret))
end




