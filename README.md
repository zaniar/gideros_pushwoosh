gideros_pushwoosh
=================

Pushwoosh plugin for Gideros

Simple example:
```
require "pushwoosh"

pushwoosh:setId("PUSHWOOSH_APP_ID", "SENDER_ID")

pushwoosh:addEventListener(Event.PUSH_RECEIVE, function(event)
	print("PushWoosh:pushReceive")
	print("  title:"..event.title)
end)
```