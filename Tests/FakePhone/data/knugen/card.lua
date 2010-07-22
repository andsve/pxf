
--[[

card = {x, y           -- position
        w, h           -- 57, 80
        hit_test(x,y)  -- returns true if point is inside card
        draging        -- if the card is currently dragged
       }
]]

function knugen:new_card(x, y)
  local card = {x = x, y = y, w = 57, h = 80}
  
  function card:hit_test(x, y)
    x = x + self.w / 2
    y = y + self.h / 2
    
    if (x < self.x) then
      return false
    elseif (x > self.x + self.w) then
      return false
    elseif (y < self.y) then
      return false
    elseif (y > self.y + self.h) then
      return false
    end
    
    return true
  end
  
  function card:draw_card(full)
    -- bg
    luagame.graphics.drawquad(self.x, self.y, self.w, self.h,
                              0, 684, 121, 169) -- texture coords
  end
  
  return card
end
