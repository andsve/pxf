
-- TODO: Localization?
knugen.suits = { "Hearts",
                 "Diamonds",
                 "Clubs",
                 "Spades" }

function knugen:gen_new_deck()
  local deck = {}
  
  for i,v in ipairs(knugen.suits) do
    for j=1,13 do
      deck[ (i-1)*13 + j] = knugen:new_card(i, j)
    end
  end
  
  return deck
end

function knugen:gen_eight_stacks(seed)
  math.randomseed(seed)
  local deck = knugen:gen_new_deck()
  local shuffled_deck = {}
  local decks = {}
  
  -- shuffle deck
  for i=1,52 do
    take_id = math.random(1,53-i)
    take_counter = 1
    for j,v in pairs(deck) do
      if (v == nil) then
        luagame:add_console("Error while shuffeling card deck!")
      end
      if take_counter == take_id then
        shuffled_deck[i] = v
        deck[j] = nil
        break
      end
      take_counter = take_counter + 1
    end
  end
  
  -- split decks
  local j = 1
  for i=1,8 do
    local n = 7
    if (i > 4) then
      n = 6
    end
    
    decks[i] = { }
    for p=1,n do
      decks[i][p] = shuffled_deck[j]
      j = j + 1
    end
  end
  
  
  return decks
end

function knugen:suit_lookup( suit_id )
  return self.suits[suit_id]
end

function knugen:new_deck(x, y, cards)
  deck = {x = x, y = y, cards = cards, drag = false, amount_visible = nil}
  
  
  function deck:draw()
    local n = #self.cards
    if (self.amount_visible) then
      n = self.amount_visible
    end
    for i=1,n do
      --[[luagame.graphics.drawquad(self.x, self.y + 18 * i, 57, 80,
                                0, 684, 121, 169) -- texture coords
                                ]]
      self.cards[i]:draw(self.x, self.y + 14 * (i-1))
    end
  end
  
  function deck:get_top()
    return self.cards[#self.cards]
  end
  
  
  -- start drag
  function deck:start_drag(hit_point)--number_of_cards)
    number_of_cards = 1 --= math.ceil((hit_point - self.y) / (80 + #self.cards * 14))
    self.amount_visible = #self.cards - number_of_cards
    
    local ret_deck = {}
    for i=1,number_of_cards do
      ret_deck[i] = self.cards[(i) + self.amount_visible]
    end
    return ret_deck
  end
  
  -- finished a drag
  function deck:drag_finished(success)
    if (success) then
      
      for i=self.amount_visible+1,#self.cards do
        self.cards[i] = nil
      end
      
    end
    
    self.amount_visible = nil
  end
  
  -- hit test
  function deck:hit_test(x, y)
    local w = 57
    local h = 80 + (#self.cards * 14)
    
    x = x + w / 2
    y = y + 80 / 2
    
    
    if (x < self.x) then
      return false
    elseif (x > self.x + w) then
      return false
    elseif (y < self.y) then
      return false
    elseif (y > self.y + h) then
      return false
    end
    
    return true
  end
  
  
  return deck
end

function knugen:new_foundation_deck(x, y)
  local deck = knugen:new_deck(x, y, {})
  
  -- try to place a deck of cards on this deck
  function deck:try_place(cards)
    
    if (#self.cards > 0) then
      -- ie can only place smaller values
      if not (cards[1].value == self:get_top().value + 1) then
        return false
        
      -- same suit
      elseif not (cards[1].suit == self:get_top().suit) then
        return false
        
      end
    else
      if not (cards[1].value == 1) then
        return false
      end
    end
    
    for i=1,#cards do
      self.cards[#self.cards+1] = cards[i]
    end
    return true
  end
  
  -- cant drag from foundation deck
  function deck:start_drag(hit_point)
    return false
  end
  
  -- only draw top card
  function deck:draw()
    if (#self.cards > 0) then
      self.cards[#self.cards]:draw(self.x, self.y)
    end
  end
  
  return deck
end

function knugen:new_cell_deck(x, y)
  local deck = knugen:new_deck(x, y, {})
  
  -- try to place a deck of cards on this deck
  function deck:try_place(cards)
    
    if (#self.cards > 0) then
      return false
    end
    
    if (#cards > 1) then
      return false
    end
    
    for i=1,#cards do
      self.cards[#self.cards+1] = cards[i]
    end
    
    return true
  end
  
  return deck
end

function knugen:new_tablau_deck(x, y, cards)
  local deck = knugen:new_deck(x, y, cards)
  for i,v in ipairs(deck) do print(i,v) end
  
  -- try to place a deck of cards on this deck
  function deck:try_place(cards)
    
    if (#self.cards > 0) then
      -- ie can only place smaller values
      if not (cards[1].value == self:get_top().value - 1) then
        return false
        
      -- same suit
      elseif (cards[1].suit == self:get_top().suit) then
        return false
        
      -- red and red
      elseif (cards[1].suit < 3 and self:get_top().suit < 3) then
        return false
        
      -- black and black
      elseif (cards[1].suit > 2 and self:get_top().suit > 2) then
        return false
      end
    end
    
    for i=1,#cards do
      self.cards[#self.cards+1] = cards[i]
    end
    return true
  end
  
  return deck
end




function knugen:new_card(suit, value)
  local card = {suit = suit, value = value}
  
  --[[function card:hit_test(x, y)
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
  end]]
  
  function card:draw(x, y, full)
    -- bg
    --texture_map01:bind()
    luagame.graphics.drawquad(x, y, 57, 80,
                              0, 684, 121, 169) -- texture coords
    
    -- suit
    luagame.graphics.drawquad(x-19, y-31, 10, 10,
                              128 + ((self.suit - 1) * 12), 777, 12, 12)
    luagame.graphics.drawquad(x+19, y+30, 10, 10, math.pi,
                              128 + ((self.suit - 1) * 12), 777, 12, 12)
                              --128 + ((self.suit - 1) * 32), 704, 32, 32) -- texture coords
    
    -- value
    luagame.graphics.drawquad(x-9, y-31, 10, 10,
                              130 + ((self.value - 1) * 10), 790, 10, 10)
    luagame.graphics.drawquad(x+9, y+30, 10, 10, math.pi,
                              130 + ((self.value - 1) * 10), 790, 10, 10)
                              
    -- temp draw suit
    --luagame:draw_font(knugen:suit_lookup(self.suit), self.x, self.y)
  end
  
  return card
end
