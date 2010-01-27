-- functions that should be moved to a helper script?

function AddWidget(name, hitbox, states, activestate)
	widget = _AddWidget(name, hitbox[1], hitbox[2], hitbox[3], hitbox[4])
	
	
	for i, w in pairs(states) do
		_AddState(widget, i)
	end
	
	_SetState(widget, activestate)
	
	return widget
end

all_widgets = {}

function GUIWidgets(widgets)
	for i, w in pairs(widgets) do
		all_widgets[#all_widgets] = {instance = AddWidget(w.name, w.hitbox, w.states, w.activestate),
		                             states = w.states
		                            }
	end
end

function DrawWidgets()
	for i, w in pairs(all_widgets) do
		activestate = _GetState(w.instance)
		w.states[activestate](w.instance)
	end
end

-- real gui user functions
function init()
	GUIWidgets({ { name = "Button1",
	               hitbox = {10, 10, 100, 40},
	               states = { idle   = function (instance) _AddQuad(instance, -127, 0, 256, 256, 0, 0, 256, 256); end,
				              active = function (instance) print("in render function for active state"); end
				            },
				   activestate = "idle"
				 }
               }
              )
end

function update(delta)
	--if (widget:MouseOver) then
	--	widget:ChangeState("over")
	--end
end
