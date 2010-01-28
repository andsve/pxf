-- functions that should be moved to a helper script?

function ReloadScript()
	return _ReloadScript()
end

function AddQuad(widget, quad, coords)
	_AddQuad(widget, quad[1], quad[2], quad[3], quad[4], coords[1], coords[2], coords[3], coords[4])
end

function AddWidget(name, hitbox, states, activestate)
	widget = _AddWidget(name, hitbox[1], hitbox[2], hitbox[3], hitbox[4])
	
	
	for i, w in pairs(states) do
		_AddState(widget, i)
	end
	
	_SetState(widget, activestate)
	_SetPosition(widget, hitbox[1], hitbox[2])
	
	return widget
end

all_widgets = {}

function GUIWidgets(widgets)
	for i, w in pairs(widgets) do
		widget = {instance = AddWidget(w.name, w.hitbox, w.states, w.activestate),
		          states = w.states,
				  activestate = w.activestate,
				  size = {w = w.hitbox[3], h = w.hitbox[4]}
				 }
		
		function widget.IsMouseOver(self)
			return _IsMouseOver(self.instance)
		end
		
		function widget.IsClicked(self)
			return _IsClicked(self.instance)
		end
		
		function widget.IsDown(self)
			return _IsDown(self.instance)
		end
		
		function widget.GetState(self)
			return self.activestate
		end
		
		function widget.SetState(self, state)
			self.activestate = state
			_SetState(self.instance, state)
		end
		
		--all_widgets[#all_widgets+1] = widget
		all_widgets[w.name] = widget
	end
end

function DrawWidgets()
	for i, w in pairs(all_widgets) do
		activestate = _GetState(w.instance)
		w.states[activestate](w.instance, w)
	end
end


-- temp render funcs
function render_button_active(instance, widget)
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

render_button_inactive = render_button_active


-- real gui user functions
theme_texture = "data/guilook.png"

function init()
	GUIWidgets({ { name = "Button1",
	               hitbox = {10, 10, 500, 50},
	               states = { active   = render_button_active,
				              inactive = render_button_inactive
				            },
				   activestate = "active"
				 },
				 { name = "Button2",
	               hitbox = {10, 100, 130, 30},
	               states = { active   = render_button_active,
				              inactive = render_button_inactive
				            },
				   activestate = "active"
				 }
               }
              )
end

function update(delta)
	if (all_widgets.Button1:IsClicked()) then
		-- nothing?
		print("Hey Button1 got clicked!")
	end
	
	if (all_widgets.Button2:IsClicked()) then
		print("Reloading GUI script!")
		ReloadScript()
	end
end
