theme_texture = "data/guilook.png"
theme_font_file = "data/alterebro_pixel.ttf"
theme_font_size = 13

----------------------------------------------------
-- standard widgets

function NewSimpleButton(_name, _position, _size, _events)

	function render_button(instance, widget)
		size = widget.size
		
		if (widget.label) then
			AddTextCentered(instance, widget.label, {size.w / 2, size.h / 2})
		end
		
		if (widget:IsDown()) then
			AddQuad(instance, {0, 0, size.w, size.h}, {244, 0, 1, 255}); -- back
			
			AddQuad(instance, {-1, -1, 3, 3}, {0, 7, 3, 3}); -- top left corner
			AddQuad(instance, {size.w -2, -1, 3, 3}, {4, 7, 3, 3}); -- top right corner
			AddQuad(instance, {2, -1, size.w-4, 3}, {3, 7, 1, 3}); -- top border
			
			AddQuad(instance, {-1, size.h-2, 3, 3}, {0, 11, 3, 3}); -- bottom left corner
			AddQuad(instance, {size.w -2, size.h-2, 3, 3}, {4, 11, 3, 3}); -- bottom right corner
			AddQuad(instance, {2, size.h-2, size.w-4, 3}, {3, 11, 1, 3}); -- bottom border
			
			AddQuad(instance, {-1, 2, 3, size.h-4}, {0, 11, 3, 0}); -- left
			AddQuad(instance, {size.w -2, 2, 3, size.h-4}, {4, 11, 3, 0}); -- left
		else
			AddQuad(instance, {0, 0, size.w, size.h}, {254, 0, 1, 255}); -- back
			
			AddQuad(instance, {-1, -1, 3, 3}, {0, 0, 3, 3}); -- top left corner
			AddQuad(instance, {size.w -2, -1, 3, 3}, {4, 0, 3, 3}); -- top right corner
			AddQuad(instance, {2, -1, size.w-4, 3}, {3, 0, 1, 3}); -- top border
			
			AddQuad(instance, {-1, size.h-2, 3, 3}, {0, 4, 3, 3}); -- bottom left corner
			AddQuad(instance, {size.w -2, size.h-2, 3, 3}, {4, 4, 3, 3}); -- bottom right corner
			AddQuad(instance, {2, size.h-2, size.w-4, 3}, {3, 4, 1, 3}); -- bottom border
			
			AddQuad(instance, {-1, 2, 3, size.h-4}, {0, 4, 3, 0}); -- left
			AddQuad(instance, {size.w -2, 2, 3, size.h-4}, {4, 4, 3, 0}); -- left

		end

	end

	states = { "active", "inactive" }
	activestate = "active"
	widget = AddWidget(_name, {_position[1], _position[2], _size[1], _size[2]},
	                   states, activestate,
					   _events, render_button)
	
	return widget
end

function NewLabeledButton(_name, _label, _position, _size, _events)
	widget = NewSimpleButton(_name, _position, _size, _events)
	widget.label = _label
	return widget
end

function NewCheckbox(_name, _position, _events)

	function render_checkbox(instance, widget)
		size = widget.size
		
		if (widget:GetState() == "checked" or widget:IsDown()) then
			AddQuad(instance, {0, 0, size.w, size.h}, {244, 0, 1, 255}); -- back
			
			AddQuad(instance, {-1, -1, 3, 3}, {0, 7, 3, 3}); -- top left corner
			AddQuad(instance, {size.w -2, -1, 3, 3}, {4, 7, 3, 3}); -- top right corner
			AddQuad(instance, {2, -1, size.w-4, 3}, {3, 7, 1, 3}); -- top border
			
			AddQuad(instance, {-1, size.h-2, 3, 3}, {0, 11, 3, 3}); -- bottom left corner
			AddQuad(instance, {size.w -2, size.h-2, 3, 3}, {4, 11, 3, 3}); -- bottom right corner
			AddQuad(instance, {2, size.h-2, size.w-4, 3}, {3, 11, 1, 3}); -- bottom border
			
			AddQuad(instance, {-1, 2, 3, size.h-4}, {0, 11, 3, 0}); -- left
			AddQuad(instance, {size.w -2, 2, 3, size.h-4}, {4, 11, 3, 0}); -- left
			
			if (widget:GetState() == "checked" and not widget:IsDown()) then
				AddQuad(instance, {3, 3, 7, 7}, {0, 16, 7, 7}); -- checker
			end
		else
			AddQuad(instance, {0, 0, size.w, size.h}, {254, 0, 1, 255}); -- back
			
			AddQuad(instance, {-1, -1, 3, 3}, {0, 0, 3, 3}); -- top left corner
			AddQuad(instance, {size.w -2, -1, 3, 3}, {4, 0, 3, 3}); -- top right corner
			AddQuad(instance, {2, -1, size.w-4, 3}, {3, 0, 1, 3}); -- top border
			
			AddQuad(instance, {-1, size.h-2, 3, 3}, {0, 4, 3, 3}); -- bottom left corner
			AddQuad(instance, {size.w -2, size.h-2, 3, 3}, {4, 4, 3, 3}); -- bottom right corner
			AddQuad(instance, {2, size.h-2, size.w-4, 3}, {3, 4, 1, 3}); -- bottom border
			
			AddQuad(instance, {-1, 2, 3, size.h-4}, {0, 4, 3, 0}); -- left
			AddQuad(instance, {size.w -2, 2, 3, size.h-4}, {4, 4, 3, 0}); -- left

		end

	end

	states = { "unchecked", "checked" }
	activestate = "unchecked"
	_size = {12, 12}
	
	widget = AddWidget(_name, {_position[1], _position[2], _size[1], _size[2]},
	                   states, activestate,
					   _events, render_checkbox)
	
	old_onclick = widget.onClick
	function widget.onClick(self)
		
		if (old_onclick) then
			old_onclick(self)
		end
		
		if (self:GetState() == "checked") then
			self:SetState("unchecked")
		else
			self:SetState("checked")
		end
	end
	
	return widget
end

function NewScroller(_name, _position, _size, _events)
	
	function render_slider(instance, widget)
		size =  {w = widget.size.w, h = widget.slider_height}
		pos_y = widget.slider_position
		
		if (widget:IsOver() and widget.last_mouse_hit) then
			AddQuad(instance, {0, pos_y, size.w, size.h}, {244, 0, 1, 255}); -- back
			
			AddQuad(instance, {-1, pos_y-1, 3, 3}, {0, 7, 3, 3}); -- top left corner
			AddQuad(instance, {size.w -2, pos_y-1, 3, 3}, {4, 7, 3, 3}); -- top right corner
			AddQuad(instance, {2, pos_y-1, size.w-4, 3}, {3, 7, 1, 3}); -- top border
			
			AddQuad(instance, {-1, pos_y+size.h-2, 3, 3}, {0, 11, 3, 3}); -- bottom left corner
			AddQuad(instance, {size.w -2, pos_y+size.h-2, 3, 3}, {4, 11, 3, 3}); -- bottom right corner
			AddQuad(instance, {2, pos_y+size.h-2, size.w-4, 3}, {3, 11, 1, 3}); -- bottom border
			
			AddQuad(instance, {-1, pos_y+2, 3, size.h-4}, {0, 11, 3, 0}); -- left
			AddQuad(instance, {size.w -2, pos_y+2, 3, size.h-4}, {4, 11, 3, 0}); -- left
		else
			AddQuad(instance, {0, pos_y, size.w, size.h}, {254, 0, 1, 255}); -- back
			
			AddQuad(instance, {-1, pos_y-1, 3, 3}, {0, 0, 3, 3}); -- top left corner
			AddQuad(instance, {size.w -2, pos_y-1, 3, 3}, {4, 0, 3, 3}); -- top right corner
			AddQuad(instance, {2, pos_y-1, size.w-4, 3}, {3, 0, 1, 3}); -- top border
			
			AddQuad(instance, {-1, pos_y+size.h-2, 3, 3}, {0, 4, 3, 3}); -- bottom left corner
			AddQuad(instance, {size.w -2, pos_y+size.h-2, 3, 3}, {4, 4, 3, 3}); -- bottom right corner
			AddQuad(instance, {2, pos_y+size.h-2, size.w-4, 3}, {3, 4, 1, 3}); -- bottom border
			
			AddQuad(instance, {-1, pos_y+2, 3, size.h-4}, {0, 4, 3, 0}); -- left
			AddQuad(instance, {size.w -2, pos_y+2, 3, size.h-4}, {4, 4, 3, 0}); -- left

		end

	end

	states = { "active", "inactive" }
	activestate = "active"
	scroll_widget = AddWidget(_name, {_position[1], _position[2], _size[1], _size[2]},
	                   states, activestate,
					   _events, render_slider)
	-- calculate slider size -> height = max(size[2] * 0.2, 20)
	scroll_widget.slider_height = math.max(_size[2] * 0.2, 20)
	
	-- save scroll position and default data
	scroll_widget.slider_position = 0
	scroll_widget.last_mouse_hit = nil
	scroll_widget.value = 0
	
	-- modify onUpdate
	old_onupdate = scroll_widget.onUpdate
	function scroll_widget.onUpdate(self)
		
		if (old_onupdate) then
			old_onupdate(self)
		end
		
		if (self:IsDraging()) then
			-- calc delta
			new_pos = self:GetMouseHit()
			
			if (not (new_pos[2] > 5 and new_pos[2] < self.size.h - 5 and new_pos[1] > -30 and new_pos[1] < 30)) then
				return -- only allow draging inside this "safe zone"
			end
			
			if (self.last_mouse_hit) then
				-- continue to drag				
				d = self.last_mouse_hit[2] - new_pos[2]
				self.slider_position = self.slider_position - d
			else
				-- start drag
				-- check if outside slider-button
				if (new_pos[2] < self.slider_position or new_pos[2] > self.slider_position + self.slider_height) then
					self.slider_position = new_pos[2] - self.slider_height / 2.0
				end
			end
			
			-- make sure we are inside our slider area
			if (self.slider_position < 0) then
				self.slider_position = 0
			elseif (self.slider_position + self.slider_height > self.size.h) then
				self.slider_position = self.size.h - self.slider_height
			end
			
			-- update value
			self:UpdateValue()
			
			self.last_mouse_hit = self:GetMouseHit()
		else
			self.last_mouse_hit = nil
		end
	end
	
	function scroll_widget.UpdateValue(self)
		self.value = self.slider_position / (self.size.h - self.slider_height)
	end
	
	function scroll_widget.IncreaseValue(self)
		self.slider_position = self.slider_position + self.slider_height * 0.2
		
		if (self.slider_position < 0) then
			self.slider_position = 0
		elseif (self.slider_position + self.slider_height > self.size.h) then
			self.slider_position = self.size.h - self.slider_height
		end
		
		self:UpdateValue()
	end
	
	function scroll_widget.DecreaseValue(self)
		self.slider_position = self.slider_position - self.slider_height * 0.2
		
		if (self.slider_position < 0) then
			self.slider_position = 0
		elseif (self.slider_position + self.slider_height > self.size.h) then
			self.slider_position = self.size.h - self.slider_height
		end
		
		self:UpdateValue()
	end
	
	-- add arrow-buttons above
	scroll_widget.arrow_up_above = NewSimpleButton(scroll_widget.name .. "arrow_up_above", {scroll_widget.position.x, scroll_widget.position.y - 32}, {scroll_widget.size.w, 15}, {onClick = function (self) self.owner:DecreaseValue(); end })
	scroll_widget.arrow_up_above.owner = scroll_widget
	scroll_widget.arrow_down_above = NewSimpleButton(scroll_widget.name .. "arrow_down_above", {scroll_widget.position.x, scroll_widget.position.y - 16}, {scroll_widget.size.w, 15}, {onClick = function (self) self.owner:IncreaseValue(); end })
	scroll_widget.arrow_down_above.owner = scroll_widget
	
	scroll_widget.arrow_up_below = NewSimpleButton(scroll_widget.name .. "arrow_up_below", {scroll_widget.position.x, scroll_widget.position.y + scroll_widget.size.h + 1}, {scroll_widget.size.w, 15}, {onClick = function (self) self.owner:DecreaseValue(); end })
	scroll_widget.arrow_up_below.owner = scroll_widget
	scroll_widget.arrow_down_below = NewSimpleButton(scroll_widget.name .. "arrow_down_below", {scroll_widget.position.x, scroll_widget.position.y + scroll_widget.size.h + 17}, {scroll_widget.size.w, 15}, {onClick = function (self) self.owner:IncreaseValue(); end })
	scroll_widget.arrow_down_below.owner = scroll_widget
	
	return scroll_widget
end

