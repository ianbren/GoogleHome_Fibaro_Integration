/*

Goal - use the ESP to poll AWS for commands targeted for the Google Home

Model

- Google Assitant or Google Home commands are written on IFTT.  
- IFTTT calls an AWS Lambda API that posts to DynamoDB
- The ESP polls the API looking for a specific record, when it is found, it is pulled down
	- the ESP maps the JSON into the API for Fibaro
	- The ESP calls the FIBARO API
	- The AWS data is wiped (to prevent it being pulled down again)
	- Loops
	

ESP8266 code to read and write to AWS DynamoDB via Lambda functions

   Setting up AWS - https://obviate.io/2015/08/05/tutorial-aws-api-gateway-to-lambda-to-dynamodb/
   Fibaro URL format - http://www.smarthome.com.au/smarthome-blog/fibaro-home-center-http-commands/
*/

/*
 

*/

/*
Process
ESP starts up, connects to local WIFI and calls AWS to clear out any AWS DynamoDB records.

loop() 
		- checks for HTTP requests - servicing if required
		- periodically check AWS for any change to the Record
			- if there is a new update is read, then the JSON is parsed out (DynamoDB has a "[" "]" array structure that has to be removed)
			- postToFibaro() is called to construct the URL to the Fibaro API
			- Once this is done then the AWS record is cleared, ready for the next Google Assitant command.
			
TODO
====
	- build out the error checking when there's a loss of comms to AWS.
	- Build out the config details of the app - change SSID etc
	- Publish the XLS that simplifes the IFTTT format
	
	
	
			
*/

