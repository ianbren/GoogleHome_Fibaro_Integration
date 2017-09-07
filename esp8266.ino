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