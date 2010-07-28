
-- Fake iPhone Simulator specifics

pxf.iphone = {}

function pxf.graphics:getscreensize()
  return 320, 480
end

--[[function pxf.iphone:getorientation()
  return 0,-1,0
end]]

pxf.mousestate = { last_pos = {-1, -1}, drag = false }

function pxf:mousedown()
  -- TODO: Fix double tap!
  mx, my = pxf.mouse.getmousepos()
  
  if not (pxf.mousestate.last_pos[1] == -1) then
    if not (pxf.mousestate.last_pos[1] == mx and pxf.mousestate.last_pos[2] == my) then
      pxf.mousestate.drag = true
    end
  end
  
  if (pxf.mousestate.drag) then
    pxf:EventDrag(pxf.mousestate.last_pos[1], pxf.mousestate.last_pos[2], mx, my)
  end
  
  pxf.mousestate.last_pos = {mx, my}
  
end

function pxf:mouseup()
  --print("Mouse released!")
  mx, my = pxf.mouse.getmousepos()
  if (pxf.mousestate.drag) then
    --print("Mouse was draged!")
    pxf:EventDrag(pxf.mousestate.last_pos[1], pxf.mousestate.last_pos[2], mx, my)
  else
    --print("Mouse was tapped!")
    pxf:EventTap(mx, my)
  end
  
  pxf:EventRelease(mx,my)
  
  pxf.mousestate.drag = false
  pxf.mousestate.last_pos = {-1, -1}
end
