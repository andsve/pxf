
LuaGame = {}
LuaGame.CoreVersion = "0.1.0"

function LuaGame:CoreInit()
  -- Debug output
  print("------------------------------------")
  print("LuaGame - Version " .. self.CoreVersion)
  print("Instance: " .. tostring(self.Instance))
  
  -- Get LuaGame class instance
  --self.Instance = _GetClassInstance()
end

