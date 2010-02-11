require("data/guibase")
require("data/basetheme")

----------------------------------------------------
-- real gui user functions

function init()
	NewLabeledButton("Button1", "Hi, im a label!", {10, 15}, {100, 15})
	NewScroller("Scroll", {60, 70}, {15, 300})
	NewSimpleButton("ReloadButton", {10, 100}, {30, 30}, {onClick = function (self) print("Reloading GUI script!"); ReloadScript(); end })
	NewCheckbox("TestCheckbox", {10, 200}, {onClick = function (self) print("oh hai, im a onClick event!"); end})
end

function update(delta)
	if (widgets.Button1:IsClicked()) then
		--print("TestCheckbox state is: " .. widgets.TestCheckbox:GetState())
		widgets.Button1:SetPosition({10, 10})
		--GetMouseHit(widgets.Button1)
		print("Scroll value: " .. tostring(widgets.Scroll.value))
		print("HONK!")
	end
end
