-- functions that should be moved to a helper script?

widgets = {}

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
	
	widgets[name] = widget
	
	return widget
end


-- engine calls these functions

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

----------------------------------------------------
-- real gui user functions
theme_texture = "data/guilook.png"

function init()
	NewSimpleButton("Button1", {10, 10}, {10, 10}, {onClick = nil, onPush = nil, onOver = nil})
	NewSimpleButton("ReloadButton", {10, 100}, {130, 30}, {onClick = nil, onPush = nil, onOver = nil})
end

function update(delta)
	if (widgets.Button1:IsClicked()) then
		print("Hey Button1 got clicked!")
	end
	
	if (widgets.ReloadButton:IsClicked()) then
		print("Reloading GUI script!")
		ReloadScript()
	end
end
