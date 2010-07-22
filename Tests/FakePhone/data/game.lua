
require("faker")

function luagame:Init()
	self.GameIdent = "Our awesome testgame!"
	self.GameVersion = "1.0"
	
	luagame:add_console("GameIdent: ^4" .. self.GameIdent)
	luagame:add_console("GameVersion: ^4" .. self.GameVersion)
	
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

function luagame:EventTap(x, y)
  luagame:add_console("Tap event, ^4x: " .. tostring(x) .. " y: " .. tostring(y))
end

function luagame:EventDoubleTap(x, y)
  luagame:add_console("Double tap event, ^4x: " .. tostring(x) .. " y: " .. tostring(y))
end

function luagame:EventDrag(x1, y1, x2, y2)
  luagame:add_console("Drag event, ^4(" .. tostring(x1) .. ", " .. tostring(y1) .. ") -> (" .. tostring(x2) .. ", " .. tostring(y2) .. "), delta: (" .. tostring(x1-x2) .. ", " .. tostring(y1-y2) .. ")")
end




