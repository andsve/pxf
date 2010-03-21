require("data/guibase")
require("data/basetheme")
require("data/messagetypes")

----------------------------------------------------
-- client gui

function init()
	NewLabeledButton("ConnectButton", "Connect to localhost:4632", {10, 15}, {140, 15})
	widgets.ConnectButton.connect_state = true
	--NewSimpleButton("ReloadButton", {10, 100}, {30, 30}, {onClick = function (self) print("Reloading GUI script!"); ReloadScript(); end })
end

function update(delta)
  if (widgets.ConnectButton:IsClicked() and widgets.ConnectButton.connect_state == false) then
	  Message(widgets.ConnectButton.instance, MessageTypes.DISCONNECT, "lol lets disconnect")
	  widgets.ConnectButton.label = "Connect to localhost:4632"
	  widgets.ConnectButton.connect_state = true
	elseif (widgets.ConnectButton:IsClicked() and widgets.ConnectButton.connect_state == true) then
	  Message(widgets.ConnectButton.instance, MessageTypes.CONNECT, "lol lets connect")
	  widgets.ConnectButton.label = "Disconnect"
	  widgets.ConnectButton.connect_state = false
	end
end
