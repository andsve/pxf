require("data/guibase")
require("data/basetheme")

----------------------------------------------------
-- real gui user functions

function init()
	NewSimpleButton("Button1", {10, 10}, {100, 15}, {onClick = function (self) print("button1 state: " .. self:GetState()); end})
	NewSimpleButton("ReloadButton", {10, 100}, {130, 30})
	NewCheckbox("TestCheckbox", {10, 200})
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
