

function knugen:create_mainmenu()
  local mainmenu = {}
  
  function mainmenu:Render(dt)
    texture_map01:bind()
    pxf.graphics.drawquad(screenw/2*dt, 0, screenw, screenh, 0, 0, 1024, 683)
    
    pxf:draw_font("main menu", screenw/2*dt, 20)
  end
  
  function mainmenu:Update( dt )
    -- nada
  end
  
  function mainmenu:EventTap(x, y)
    -- nada
  end

  function mainmenu:EventDoubleTap(x, y)
    -- nada
  end

  function mainmenu:EventDrag(x1, y1, x2, y2)
    -- nada
  end

  function mainmenu:EventRelease(x, y)  
    -- nade
  end
  
  return mainmenu
end

function knugen:create_settingsmenu()
  local menu = {}
  
  function menu:Render(dt)
    texture_map01:bind()
    pxf.graphics.drawquad(screenw/2*dt, 0, screenw, screenh, 0, 0, 1024, 683)
    
    pxf:draw_font("settings", screenw/2*dt, 20)
  end
  
  function menu:Update( dt )
    -- nada
  end
  
  function menu:EventTap(x, y)
    -- nada
  end

  function menu:EventDoubleTap(x, y)
    -- nada
  end

  function menu:EventDrag(x1, y1, x2, y2)
    -- nada
  end

  function menu:EventRelease(x, y)  
    -- nade
  end
  
  return menu
end


function knugen:create_menu()
  local menu = {}
  menu.windows = { main     = knugen:create_mainmenu(),
                   settings = knugen:create_settingsmenu() }
  menu.window_stack = {"main", "settings"}
  menu.window_old = "main"
  menu.translation = 1.0
  
  function menu:GetCurrentWindow()
    return self.windows[self.window_stack[#self.window_stack]]
  end
  
  function menu:GetOldWindow()
    return self.windows[self.window_old]
  end
  
  function menu:ChangeWindow(new_window_name)
    self.window_old = self.window_stack[#self.window_stack]
    self.window_stack[#self.window_stack + 1] = new_window_name
    self.translation = 0.0
  end
  
  function menu:GoBack()
    if (#self.window_stack > 1) then
      self.window_old = self.window_stack[#self.window_stack] -- set "old" current
      self.window_stack[#self.window_stack] = nil --table.remove(self.window_stack)     -- remove old from window stack
      self.translation = 0.0
    end
  end
  
  function menu:Render()
  
    -- winow translations
    if (self.translation < 1.0) then
      self:GetOldWindow():Render(self.translation)
      self:GetCurrentWindow():Render(1.0 - self.translation)
      
      self.translation = self.translation + 0.001
    else
      self:GetCurrentWindow():Render(self.translation)
    end
    if (self.translation > 1.0) then
      self.translation = 1.0
    end
    
    
  end
  
  function menu:Update( dt )
    self:GetCurrentWindow():Update(dt)
  end
  
  function menu:EventTap(x, y)
    self:GoBack()
    --self:GetCurrentWindow():EventTap(x, y)
  end

  function menu:EventDoubleTap(x, y)
    self:GetCurrentWindow():EventDoubleTap(x, y)
  end

  function menu:EventDrag(x1, y1, x2, y2)
    self:GetCurrentWindow():EventDrag(x1, y1, x2, y2)
  end

  function menu:EventRelease(x, y)  
    self:GetCurrentWindow():EventRelease(x, y)
  end
  
  return menu
end
