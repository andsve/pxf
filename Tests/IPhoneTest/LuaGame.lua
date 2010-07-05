
luagame = {}
luagame.CoreVersion = "0.1.0"

function luagame:CoreInit()
  -- Debug output
  print("------------------------------------")
  print("LuaGame - Version " .. self.CoreVersion)
  print("Instance: " .. tostring(self.Instance))
  
  -- Test preload
  debug_font_texture = luagame.resources.loadtexture("debug_font.png")
end

function luagame:draw_font(str, x, y)
  
  debug_font_texture:bind()
  luagame.graphics.translate(x, y)
	strlen = #str
	char_w = 8
	
	for i=1,strlen do
	  -- calculate tex coords
	  index = string.byte(str, i)
	  
	  s = math.fmod(index, 16) * 16
	  t = math.floor(index / 16) * 16
	  
	  -- debug
	  --print(string.char(tostring(string.byte(str, i))) .. " -> " .. tostring(string.byte(str, i)))
	  
	  -- draw quad
	  luagame.graphics.drawquad((i-1)*char_w, 0, 16, 16, s, t, 16, 16)
	end
	
	luagame.graphics.translate(-x, -y)
end

