-- Assumptions
-- Virtual Device (VD) has IPAddress and TCPPort set to that of the ESP8266
-- That there are three fields on the VD
-- 	a) lbAWSCallCount - contains the number of calls to the ESP8266 - used for debugging
--	b) lbCommandCount - the number of commands that have come down via the ESP8266
--	c) lbLastCommand - the text of the last command that came down

-- this LUA script is configured to simplify the garbage collection - all activities happen inside the "main()"

-- The ESP8266 will pass back an HTTP 204 if there is nothing to action on, 
-- otherwise an HTTP 200 will be returned and the payload will be JSON in the format
--
--{
--	"fbID": "94",
--	"fbType":"Scene/Device",
--	"fbAction": "turnOff",
--	"fbPayload": " ",
--	"fbComments": "Turn Lounge Light Off",
--	"fbGuid": "this-is-a-guid-field"
--}	
-- fbGUid is not used by the Fibaro - it is used by the ESP8266 as a shared secret to prevent unexpected records being pulled down
-- fbType is either "Device" or Scene
-- fbAction is the the action to take



if (MyObject == nil) then MyObject = {
	-- System variables
    Web = Net.FHttp(fibaro:getValue(fibaro:getSelfId(), 'IPAddress'), fibaro:getValue(fibaro:getSelfId(), 'TCPPort')) ,

	-- Main code
	main = function (self)
		local thisdevice = fibaro:getSelfId()
		local payload = "/"
		response, status, errorCode = self.Web: GET (payload)

		if tonumber (errorCode) ~= 0 then
			fibaro: debug ('Error: Cannot connect to Web server, status =' .. status .. ', errorCode =' .. errorCode)
			return false
		end
		if 	(response == nil) then
			-- no content
			fibaro: debug ('Response  is Nil : '.. Response)
			return false
		end
			
		fibaro:call(thisdevice, "setProperty", "ui.lbAWSCallCount.value", fibaro:getValue(thisdevice, "ui.lbAWSCallCount.value")+1)
			
		-- HTTP 204 - "No Data" 
		-- there's nothing to process, so we'll just get out of this loop
		if tonumber (status) == 204  then
			fibaro:debug("No AWS Message to process")
			fibaro:call(thisdevice, "setProperty", "ui.lbLastCommand.value", "No AWS Message to process")
			return true
		else

		-- HTTP 200 - so there's something worth looking at 
		if tonumber (status) == 200  then
			fibaro:call(thisdevice, "setProperty", "ui.lbCommandCount.value", fibaro:getValue(thisdevice, "ui.lbCommandCount.value")+1)
			local jsonTable = json.decode(response)
				
			if jsonTable.fbID == "0" then
				-- we shouldn't see a zero, as it should come back as a HTTP204 - so maybe not needed				
				fibaro:call(thisdevice, "setProperty", "ui.lbLastCommand.value", "No Message")
				fibaro:debug("No Message") 
			else

				fibaro:call(thisdevice, "setProperty", "ui.lbLastCommand.value", jsonTable.fbComments)
				fibaro:debug(response) -- send JSON to debug

				-- running a scene on Fibaro
				if jsonTable.fbType == "Scene" then
					-- a scene is to be called
					fibaro:startScene(jsonTable.fbID)
					fibaro:call(jsonTable.fbID, jsonTable.fbAction, "Last Command: " .. jsonTable.fbPayload )
				end
			
				-- we're going to be explicit here - better to be safe than sorry
				if jsonTable.fbType == "Device" then
					-- ok, so it's a Device of some sorts
					fibaro:call(jsonTable.fbID, jsonTable.fbAction, jsonTable.fbPayload )
				end
				-- if we had another value come through - either deal with it explicitly or not... your choice!			
			end
		end
	end
end
}
fibaro:debug ("Function successfully loaded in memory")
end

-- Start
MyObject:main ()
