-- functions that should be moved to a helper script?

function ReloadScript()
	return _ReloadScript()
end

function AddQuad(widget, quad, coords)
	_AddQuad(widget, quad[1], quad[2], quad[3], quad[4], coords[1], coords[2], coords[3], coords[4])
end

function AddWidget(name, _hitbox, _states, _activestate, events, _render)
	widget = {states = _states,
			  hitbox = _hitbox,
			  activestate = _activestate,
			  size = {w = _hitbox[3], h = _hitbox[4]},
			  render = _render,
			  onClick = events.onClick,
			  onPush = events.onPush,
			  onOver = events.onOver
			 }
	widget.instance = _AddWidget(name, _hitbox[1], _hitbox[2], _hitbox[3], _hitbox[4])
	
	
	for i, _s in pairs(_states) do
		_AddState(widget.instance, _s)
	end
	
	_SetState(widget.instance, _activestate)
	_SetPosition(widget.instance, _hitbox[1], _hitbox[2])
	
	-- setup calls
	
	function widget.IsOver(self)
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
	
	return widget
end

widgets = {}

function CreateGUIWidgets(_widgets)
	for i, w in pairs(_widgets) do
		widget = AddWidget(w.name, w.hitbox, w.states, w.activestate, w.events, w.render)
		
		widgets[w.name] = widget
	end
end

function DrawWidgets()
	for i, w in pairs(widgets) do
		w.render(w.instance, w)
	end
end

function UpdateWidgets(delta)
	for i, w in pairs(widgets) do
		if (w.onClick) then
			if (w:IsClicked()) then
				w.onClick(widget)
			end
		end
		
		if (w.onDown) then
			if (w:IsDown()) then
				w.onDown(widget)
			end
		end
		
		if (w.onOver) then
			if (w:IsOver()) then
				w.onOver(widget)
			end
		end
	end
	
	update(delta)
end


-- temp render funcs
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


-- real gui user functions
theme_texture = "data/guilook.png"

function init()
	CreateGUIWidgets({ { name = "Button1",
					     hitbox = {10, 10, 10, 10},
					     states = { "active", "inactive" },
					     activestate = "active",
						 render = render_button,
						 events = { onClick = function (widget) print("Hey now!"); end, 
						            onDown = nil, onOver = nil}
					 },
					   { name = "ReloadButton",
						 hitbox = {10, 100, 130, 30},
						 states = { "active", "inactive" },
					     activestate = "active",
						 render = render_button,
						 events = { onClick = nil, onDown = nil, onOver = nil}
					   }
				     }
				    )
end

function update(delta)
	if (widgets.Button1:IsClicked()) then
		-- nothing?
		print("Hey Button1 got clicked!")
	end
	
	if (widgets.ReloadButton:IsClicked()) then
		print("Reloading GUI script!")
		ReloadScript()
	end
end
