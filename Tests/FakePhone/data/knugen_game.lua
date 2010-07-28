require("faker")

knugen = {}

require("knugen/menu")
require("knugen/card")
--require("knugen/knugen")
--require("knugen/harpan")

function luagame:Init()
	self.GameIdent = "Knugen"
	self.GameVersion = "1.0"
	
	luagame:add_console("GameIdent: ^4" .. self.GameIdent)
	luagame:add_console("GameVersion: ^4" .. self.GameVersion)
	
	screenw, screenh = luagame.graphics.getscreensize()
	luagame:add_console("Screen size: ^4" .. tostring(screenw) .. "x" .. tostring(screenh))
	
	menu = knugen:create_menu()
	--game = knugen:create_game(4356)
	
	host = "*"
  port = 23
  
	socket = require("socket")
  luagame:add_console(socket._VERSION)
  luagame:add_console("Binding to host '" ..host.. "' and port " ..port.. "...")
  s = socket.tcp()
  s:bind(host, port)
  s:listen()
  c = s:accept()
  c:settimeout(0)
  luagame:add_console("Connected. Here is the stuff:")
  l, e = c:receive()
	
end

function luagame:PreLoad()
	texture_map01 = luagame.resources.loadtexture("knugen_map01.png")
end

function luagame:Update(dt)
	--print("Time to UPDATE our game with '" .. tostring(dt) .. "'")
	menu:Update(dt)
end

function luagame:Render()
	
	if (l) then
	  luagame:add_console(l)
  end
	l, e = c:receive()
	
	menu:Render()
	
	-- render console
	luagame.graphics.loadidentity()
  luagame.graphics.translate(screenw / 2.0, screenh / 2.0)
  luagame.graphics.rotate(math.pi / 2)
  luagame.graphics.translate(-screenh / 2.0, -screenw / 2.0)
  luagame.console.cut_off_width = screenh
  luagame:draw_console(screenh, screenw)
  
end


function luagame:TextInput(str)
  luagame:add_console("> " .. str)
  ret = runstring("return " .. str)
  luagame:add_console("^4" .. tostring(ret))
end

function luagame:EventTap(x, y)
  --luagame:add_console("Tap event, ^4x: " .. tostring(x) .. " y: " .. tostring(y))
  if not luagame:console_taptest(x, y) then
    menu:EventTap(x, y)
  end
end

function luagame:EventDoubleTap(x, y)
  --luagame:add_console("Double tap event, ^4x: " .. tostring(x) .. " y: " .. tostring(y))
  menu:EventDoubleTap(x, y)
end

function luagame:EventDrag(x1, y1, x2, y2)
  --luagame:add_console("Drag event, ^4(" .. tostring(x1) .. ", " .. tostring(y1) .. ") -> (" .. tostring(x2) .. ", " .. tostring(y2) .. "), delta: (" .. tostring(x1-x2) .. ", " .. tostring(y1-y2) .. ")")
  menu:EventDrag(x1, y1, x2, y2)
end

function luagame:EventRelease(x, y)  
  --luagame:add_console("Release event, ^4x: " .. tostring(x) .. " y: " .. tostring(y))
  menu:EventRelease(x, y)
end



