
luagame.CoreVersion = "0.1.0"

function luagame:CoreInit()
  luagame:init_console(10)
  
  -- Debug output
  luagame:add_console("LuaGame - Version ^4" .. self.CoreVersion)
  luagame:add_console("Instance: ^4" .. tostring(self.Instance))
  
  -- Test preload
  debug_font_texture = luagame.resources.loadtexture("debug_font.png")
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

-- Debugging console
function luagame:init_console(max_lines)
  self.console = {buffer = {}, max_lines = max_lines, current_input = 0}
end

function luagame:add_console(str, skip_stdout)
  -- print to normal console
  if (skip_stdout == nil) then
    print(str)
  end
  
  -- move buffer up
  if self.console.current_input == self.console.max_lines then
    for i=1,(self.console.current_input - 1) do
      self.console.buffer[i] = self.console.buffer[i+1]
    end
  end
  
  -- add new str
  self.console.current_input = self.console.current_input + 1
  if self.console.current_input > self.console.max_lines then
    self.console.current_input = self.console.max_lines
  end
  
  -- Add up until max width
  screenw, screenh = luagame.graphics.getscreensize()
  line_w = #str*8 + 8
  if (line_w > screenw) then
    cut_place = math.ceil((screenw / line_w) * #str)
    s = string.sub(str, 1, cut_place)
    e = string.sub(str, cut_place + 1)
    self.console.buffer[self.console.current_input] = s
    luagame:add_console("^4 " .. e, true)
  else
    self.console.buffer[self.console.current_input] = str
  end
end

function luagame:draw_console()
  screenw, screenh = luagame.graphics.getscreensize()
  
  -- bg
  console_h = 10 * self.console.max_lines + 6
  luagame.graphics.unbindtexture()
  luagame.graphics.setcolor(0, 0, 0, 0.5)
  luagame.graphics.drawquad(screenw / 2, (console_h / 2), screenw, console_h)
  luagame.graphics.setcolor(1, 1, 1, 1.0)
  luagame.graphics.drawquad(screenw / 2, console_h - 1, screenw, 1)
  
  -- text
  x = 8
  --y = 26
  y = 8
  for i=1,(self.console.current_input) do
    luagame:draw_font(self.console.buffer[i], x, y+((i-1) * 10))
  end
  
  -- draw line count
  counttext = "[" .. tostring(self.console.current_input) .. "," .. tostring(self.console.max_lines) .. "]"
  luagame:draw_font(counttext, screenw - #counttext*8, y)
end

-- Font/text (using charmap) rendering
function luagame:draw_font(str, x, y)
  
  debug_font_texture:bind()
  luagame.graphics.setcolor(1, 1, 1, 1) -- TODO: Should use some kind of getcolor before drawing, so it can be restored here
  luagame.graphics.translate(x, y)
	strlen = #str
	char_w = 8
	
	color_indicator = "^"
	change_color = false
	char_counter = 0
	
	for i=1,strlen do
	  -- calculate tex coords
	  index = string.byte(str, i)
	  
	  s = math.fmod(index, 16) * 16
	  t = math.floor(index / 16) * 16
	  
	  -- debug
	  --print(string.char(tostring(string.byte(str, i))) .. " -> " .. tostring(string.byte(str, i)))
	  
	  -- change color?
	  if index == string.byte(color_indicator, 1) then
	    change_color = true
    else
      if change_color then
        
        -- Color indexes
        if string.char(tostring(string.byte(str, i))) == "0" then
          luagame.graphics.setcolor(0, 0, 0, 1)
        elseif string.char(tostring(string.byte(str, i))) == "1" then
          luagame.graphics.setcolor(1, 0, 0, 1)
        elseif string.char(tostring(string.byte(str, i))) == "2" then
          luagame.graphics.setcolor(0, 1, 0, 1)
        elseif string.char(tostring(string.byte(str, i))) == "3" then
          luagame.graphics.setcolor(0, 0, 1, 1)
        elseif string.char(tostring(string.byte(str, i))) == "4" then
          luagame.graphics.setcolor(1.0, 0.3, 0.3, 1)
        else
          luagame.graphics.setcolor(1, 1, 1, 1)
        end
        
        change_color = false
      else
  	    -- draw quad
  	    luagame.graphics.drawquad((char_counter)*char_w, 0, 16, 16, s, t, 16, 16)
  	    char_counter = char_counter + 1
	    end
    end
	end
	
	luagame.graphics.translate(-x, -y)
	luagame.graphics.setcolor(1, 1, 1, 1) -- TODO: Should use some kind of getcolor before drawing, so it can be restored here
end

function luagame.graphics:newsprite(tex,cw,ch,freq)
	new_sprite = {}
	new_sprite.position = { x = 0, y = 0 }

	texture = tex
	cellwidth = cw
	cellheight = ch
	current_cell = 0

	tex_w,tex_h = texture:getsize()
	max_cellsw = tex_w / cw
	max_cellsh = tex_h / ch
	max_cells = max_cellsw * max_cellsh

	--print(max_cells)
	
	x0 = cellwidth * (current_cell % max_cellsw)
	y0 = cellheight * (current_cell / max_cellsh)

	--print(current_cell % max_cellsw)
	--print(current_cell / max_cellsh)

	function new_sprite:draw()
		texture:bind()
		
		luagame.graphics.drawquad(50, 150, cellwidth, cellheight, x0, y0, x0 + cellwidth, y0 + cellheight)
	end

	function new_sprite:update(dt)
		
	end

	return new_sprite
end
