
widgets = {}

function ReloadScript()
	return _ReloadScript()
end

function AddQuad(widget, quad, coords)
	_AddQuad(widget, quad[1], quad[2], quad[3], quad[4], coords[1], coords[2], coords[3], coords[4])
end

function AddWidget(_name, _hitbox, _states, _activestate, events, _render)
	if (events == nil) then
		events = {}
	end
	
	widget = {name = _name,
			  states = _states,
			  hitbox = _hitbox,
			  activestate = _activestate,
			  size = {w = _hitbox[3], h = _hitbox[4]},
			  render = _render,
			  onClick = events.onClick,
			  onPush = events.onPush,
			  onOver = events.onOver
			 }
	widget.instance = _AddWidget(_name, _hitbox[1], _hitbox[2], _hitbox[3], _hitbox[4])
	
	
	for i, _s in pairs(_states) do
		_AddState(widget.instance, _s)
	end
	
	_SetState(widget.instance, _activestate)
	_SetPosition(widget.instance, _hitbox[1], _hitbox[2])
	
	-- setup calls
	
	function widget.SetPosition(self, pos)
		_SetPosition(self.instance, pos[1], pos[2])
	end
	
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
	
	function widget.GetMouseHit(self)
		x, y = _GetMouseHit(self.instance)
		return {x, y}
	end
	
	widgets[_name] = widget
	
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
				w.onClick(w)
			end
		end
		
		if (w.onDown) then
			if (w:IsDown()) then
				w.onDown(w)
			end
		end
		
		if (w.onOver) then
			if (w:IsOver()) then
				w.onOver(w)
			end
		end
	end
	
	update(delta)
end

