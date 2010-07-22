require("faker")

knugen = {}

require("knugen/card")

function luagame:Init()
	self.GameIdent = "Knugen"
	self.GameVersion = "1.0"
	
	luagame:add_console("GameIdent: ^4" .. self.GameIdent)
	luagame:add_console("GameVersion: ^4" .. self.GameVersion)
	
	screenw, screenh = luagame.graphics.getscreensize()
	luagame:add_console("Screen size: ^4" .. tostring(screenw) .. "x" .. tostring(screenh))
	
	cards = {knugen:new_card(50, 100),
	         knugen:new_card(50, 140),
	         knugen:new_card(100, 100)}
	
	last_mouse_dra = {drag = false, x1 = 0, y1 = 0, x2 = 0, y2 = 0, dx = 0, dy = 0}
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
	
	luagame.graphics.translate(-screenh / 2.0, -screenw / 2.0)
  
  tx1 = last_mouse_dra.x1
  ty1 = last_mouse_dra.y1
  tx2 = last_mouse_dra.x2
  ty2 = last_mouse_dra.y2
  
	if (last_mouse_dra.drag) then
	  for i=1,#cards do
      if cards[i]:hit_test(tx1, ty1) then
        cards[i].x = tx2
        cards[i].y = ty2
        break
      end
	  end
  end
	
	for i=1,#cards do
	  cards[i]:draw_card(true)
  end
	
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
  
  -- reset mouse drag
  last_mouse_dra.drag = false
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
  last_mouse_dra.x1 = x1
  last_mouse_dra.y1 = y1
  last_mouse_dra.x2 = x2
  last_mouse_dra.y2 = y2
  last_mouse_dra.dx = x1-x2
  last_mouse_dra.dy = y1-y2
  last_mouse_dra.drag = true
  luagame:add_console("Drag event, ^4(" .. tostring(x1) .. ", " .. tostring(y1) .. ") -> (" .. tostring(x2) .. ", " .. tostring(y2) .. "), delta: (" .. tostring(x1-x2) .. ", " .. tostring(y1-y2) .. ")")
end




