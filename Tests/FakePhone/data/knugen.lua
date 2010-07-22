
require("faker")

function luagame:Init()
	self.GameIdent = "Knugen"
	self.GameVersion = "1.0"
	
	luagame:add_console("GameIdent: ^4" .. self.GameIdent)
	luagame:add_console("GameVersion: ^4" .. self.GameVersion)
	
	screenw, screenh = luagame.graphics.getscreensize()
	luagame:add_console("Screen size: ^4" .. tostring(screenw) .. "x" .. tostring(screenh))
	
end

function luagame:PreLoad()
	texture_map01 = luagame.resources.loadtexture("knugen_map01.png")
end

function luagame:Update(dt)
	--print("Time to UPDATE our game with '" .. tostring(dt) .. "'")
end

function luagame:Render()
	
	-- render background
	texture_map01:bind()
	luagame.graphics.translate(screenw / 2.0, screenh / 2.0)
	luagame.graphics.rotate(math.pi / 2)
	luagame.graphics.drawquad(0, 0,screenh,screenw, 0, 0, 1024, 683)
	--luagame.graphics.drawquad(screenw / 2.0, screenh / 2.0,screenw,screenh)
	
	-- test draw texture 2
	--[[test_texture2:bind()
	
	luagame.graphics.loadidentity()
	luagame.graphics.translate(screenw / 2.0, screenh / 2.0)
	luagame.graphics.drawquad(-64, -64, 256, 128)
	
	-- back to texture 1
	test_texture:bind()
	luagame.graphics.loadidentity()
	luagame.graphics.translate(screenw / 2.0, screenh / 2.0)
	luagame.graphics.drawquad(0, 0, 32, 32, 0, 0, 16, 16)
	]]
	
	-- render debug text
	luagame.graphics.loadidentity()
	ox,oy,oz = luagame.iphone:getorientation()
	if (math.abs(ox) < math.abs(oy)) then
	  luagame.graphics.translate(screenw / 2.0, screenh / 2.0)
	  luagame.graphics.rotate(math.pi / 2)
	  luagame.graphics.translate(-screenh / 2.0, -screenw / 2.0)
	  luagame.console.cut_off_width = screenh
	  luagame:draw_console(screenh, screenw)
  else
    luagame.console.cut_off_width = screenw
	  luagame:draw_console(screenw, screenh)
	end
	  
	
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
  luagame:console_taptest(x, y)
end

function luagame:EventDoubleTap(x, y)
  luagame:add_console("Double tap event, ^4x: " .. tostring(x) .. " y: " .. tostring(y))
end

function luagame:EventDrag(x1, y1, x2, y2)
  luagame:add_console("Drag event, ^4(" .. tostring(x1) .. ", " .. tostring(y1) .. ") -> (" .. tostring(x2) .. ", " .. tostring(y2) .. "), delta: (" .. tostring(x1-x2) .. ", " .. tostring(y1-y2) .. ")")
end




