require("data/guibase")
require("data/basetheme")

----------------------------------------------------
-- real gui user functions

function init()
	NewSimpleButton("Button1", {10, 10}, {100, 15})
	NewSimpleButton("ReloadButton", {10, 100}, {130, 30}, {onClick = function (self) print("Reloading GUI script!"); ReloadScript(); end })
	NewCheckbox("TestCheckbox", {10, 200}, {onClick = function (self) print("oh hai, im a onClick event!"); end})
end

function update(delta)
	if (widgets.Button1:IsClicked()) then
		--print("TestCheckbox state is: " .. widgets.TestCheckbox:GetState())
		widgets.Button1:SetPosition({10, 15})
		GetMouseHit(widgets.Button1)
	end
end
