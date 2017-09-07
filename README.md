# Google Home to Fibaro Integration

This shows integration from Google Home (GH) (or Assistant) through IFTTT to Fibaro HC2.

The infrastructure required
1. a Google Home on the Network
2.  Fibaro HC2 on the Network
3.  an ESP8266 (I used a WEMOS D1 Mini) with associated code
4.  a Free Tier of AWS containing
    - a DynamoDB with one recored - the command to be executed
    - a Lambda to control access to the record
    - an AWS API to give web access for read and write
5.  an IFTTT that'll hook the Asistant Command to the AWS API endpoint.

Flow
- the user requests "hey google turn the lounge light on"
- GH hooks into the IFTTT command
- IFTTT calls the AWS API end Point with the payload requred in JSON format
- the Payload is written to the DynamoDB record via API and Lambda
- The ESP8266 (ESP) polls the AWS API periodically (per second in this version) and looks for a nominated record number (myID).
	-	when a non-zero ID is found (i.e. when a record has been posted by IFTTT) then 
			- the ESP pulls down the JSON record and processes it
			- Determines if this is a Device or a Scene to be run
			- directly updates the appropriate object in Fibaro
			- clears the AWS record ready for the next update
			
	_Benefits of the approach_
	- the Fibaro does not have to poll the ESP - this can take a number of seconds, on top of the polling time that the ESP takes to contact AWS.
	- the permissions of the Fibaro can be used to limit the scope of the ESP.  This can prevent an error in the AWS record propogating to Fibaro
	- we can use the simple Fibaro API, rather than the REST API
	
[end of file]


