
luagame = {}
luagame.CoreVersion = "0.1.0"

function luagame:CoreInit()
  -- Debug output
  print("------------------------------------")
  print("LuaGame - Version " .. self.CoreVersion)
  print("Instance: " .. tostring(self.Instance))
  self:Test()
end

