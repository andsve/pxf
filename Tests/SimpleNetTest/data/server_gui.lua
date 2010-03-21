require("data/guibase")
require("data/basetheme")
require("data/messagetypes")

----------------------------------------------------
-- server gui

function init()
	NewLabeledButton("ShutdownButton", "Shutdown Server", {10, 15}, {140, 15})
end

function update(delta)
	if (widgets.ShutdownButton:IsClicked()) then
	  Message(widgets.ShutdownButton.instance, MessageTypes.DISCONNECT, "goodbye")
		--print("TestCheckbox state is: " .. widgets.TestCheckbox:GetState())
	end
end
