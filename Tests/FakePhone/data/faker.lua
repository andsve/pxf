
-- Fake iPhone Simulator specifics

luagame.iphone = {}

function luagame.graphics:getscreensize()
  return 320, 480
end

--[[function luagame.iphone:getorientation()
  return 0,-1,0
end]]

luagame.mousestate = { last_pos = {-1, -1}, drag = false }

function luagame:mousedown()
  -- TODO: Fix double tap!
  mx, my = luagame.mouse.getmousepos()
  
  if not (luagame.mousestate.last_pos[1] == -1) then
    if not (luagame.mousestate.last_pos[1] == mx and luagame.mousestate.last_pos[2] == my) then
      luagame.mousestate.drag = true
    end
  end
  
  if (luagame.mousestate.drag) then
    luagame:EventDrag(luagame.mousestate.last_pos[1], luagame.mousestate.last_pos[2], mx, my)
  end
  
  luagame.mousestate.last_pos = {mx, my}
  
end

function luagame:mouseup()
  --print("Mouse released!")
  mx, my = luagame.mouse.getmousepos()
  if (luagame.mousestate.drag) then
    --print("Mouse was draged!")
    luagame:EventDrag(luagame.mousestate.last_pos[1], luagame.mousestate.last_pos[2], mx, my)
  else
    --print("Mouse was tapped!")
    luagame:EventTap(mx, my)
  end
  
  luagame:EventRelease(mx,my)
  
  luagame.mousestate.drag = false
  luagame.mousestate.last_pos = {-1, -1}
end
