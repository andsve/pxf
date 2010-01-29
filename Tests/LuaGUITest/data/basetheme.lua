theme_texture = "data/guilook.png"

----------------------------------------------------
-- standard widgets

function NewSimpleButton(_name, _position, _size, _events)

	function render_button(instance, widget)
		size = widget.size
		
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

