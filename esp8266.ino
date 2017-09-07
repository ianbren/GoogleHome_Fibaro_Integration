/*
   Setting up AWS - https://obviate.io/2015/08/05/tutorial-aws-api-gateway-to-lambda-to-dynamodb/
   Fibaro URL format - http://www.smarthome.com.au/smarthome-blog/fibaro-home-center-http-commands/
*/

/*
   V3 - last one that uses Fibaro to drive schedule
   V4 - polls AWS every second and directly calls Fibaro to post the details to the Device
     - initial version posts to a Virtual Device and then have Fibaro read the details.
     - Structure
     - fbID - the Fibaro Device to be actioned - Zero is no action
     - fbType - "Scene" or "Device" - taken from AWS record
     - fbAction - what is to be done (turnOn/turnOff etc)
     fbGUID - not used, but ready for when we confirm the record hasn't been updated outside this process

   V5 - Reviewing the URL pattern used to send commands
      -

*/

/*
Process
ESP starts up, connects to local WIFI and calls AWS to clear out any AWS DynamoDB records.

loop() 
		- checks for HTTP requests - servicing if required
		- periodically check AWS for any change to the Record
			- if there is a new update is read, then the JSON is parsed out (DynamoDB has a "[" "]" array structure that has to be removed)
			- postToFibaro() is called to construct the URL to the Fibaro API
			- Once this is done then the AWS record is cleared.
			
TODO
====
	- build out the error checking when there's a loss of comms to AWS.
	- Build out the config details of the app - change SSID etc
	- Publish the XLS that simplifes the IFTTT format
	
	
	
			
*/

