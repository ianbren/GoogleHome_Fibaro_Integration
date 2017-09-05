# Google Home to Fibaro Integration

This shows integration from Google Home (GH) (or Assistant) through IFTTT to Fibaro HC2.

The infrastructure required
1. - a Google Home on the Network
2. - Fibaro HC2 on the Network
3. - an ESP8266 (I used a WEMOS D1 Mini) with associated code
4. - a Free Tier of AWS containing
  - a DynamoDB with one recored - the command to be executed
  - a Lambda to control access to the record
  - an AWS API to give web access for read and write
5. - an IFTTT that'll hook the Asistant Command to the AWS API endpoint.

Flow
- the user requests "hey google turn the lounge light on"
- GH hooks into the IFTTT command
- IFTTT calls the AWS API end Point with the payload requred in JSON format
- the Payload is written to the DynamoDB record via API and Lambda
- The Fibaro polls the ESP8266 periodically 
  - The ESP8266 (on request from the Fibaro) calls the  API "GET" command, looking for a change in the DynamoDB JSON record
  - If no change is found an HTTP 204 is passed back from the ESP8266 to the Fibaro
  - If a change is found then an HTTP 200 is passed back from the ESP8266 to the Fibaro, with a JSON payload  mapped to a Fibaro Device command
  - The ESP8266 clears the DynamoDB record
  
  
- 

