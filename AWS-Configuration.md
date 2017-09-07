#AWS Configuration Details

The AWS configuration consists of three components
	- DynamoDB - where a request is stored as a record in the database
	- The Application API 
		- where the ESP8266 calls to get the Record as a JSON, or Updates a record
		- where IFTTT posts data
	- The AWS Lamdba - the "glue" between the API and DynamoDB.
	
	The setup for this is given at https://obviate.io/2015/08/05/tutorial-aws-api-gateway-to-lambda-to-dynamodb/
	
	
	Note the DynamoDB record is of the following  
	{
	"myID": "1",
	"fbID": "98",
	"fbType":"Device",
	"fbAction": "turnOff",
	"fbPayload": " ",
	"fbComments": "Turn Fountain Off",
	"fbGuid": "this-is-a-guid-field"
}	   

Where 
	"myID"				- 	The single record used to transfer data
	"fbID"				-		The Fibaro ID for the object to be called
	"fbType"			-		Device/Scene - depending on what is being called on the Fibaro.
	"fbAction":		-		The Fibaro Action - turnOn, turnOff, run, start etc
	"fbPayload":	-		Additional Parameters if needed (e.g. fbAction = pressButton, fbPayload = "3" for button 3
	"fbComments": -		No use given
	"fbGuid": 		- 	A GUID or shared secret stored in ESP8266 and IFTTT.
	
	
Useful Links - Fibaro URL http://www.smarthome.com.au/smarthome-blog/fibaro-home-center-http-commands/

