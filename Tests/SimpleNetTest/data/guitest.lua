require("data/guibase")
require("data/basetheme")

----------------------------------------------------
-- real gui user functions

function init()
	NewLabeledButton("Button1", "Connect", {10, 15}, {100, 15})
	--NewSimpleButton("ReloadButton", {10, 100}, {30, 30}, {onClick = function (self) print("Reloading GUI script!"); ReloadScript(); end })
end

function update(delta)
	if (widgets.Button1:IsClicked()) then
		--print("TestCheckbox state is: " .. widgets.TestCheckbox:GetState())
	end
end
