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

#include "config.h" // the config details for the app

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>

//LED details
int ledState = LOW;
unsigned long previousLedMillis = 0;
const long ledInterval = 500;


unsigned long previousMillis = 0;        // will store last time AWS was polled
const long interval = 500;           // interval at which to poll AWS and update Fibaro
//  there's a few seconds within the AWS call - so this just stops a race condition


// ****** Fibaro Details ******
HTTPClient http;  // used to send info to Fibaro
// different Fibaro URLs for a Device and a String call
char fbDeviceString[] = "http://%s/api/callAction?deviceID=%d&name=%s&arg1=%s";
char fbSceneString[] = "http://%s/api/sceneControl?id=%d&action=%s&arg1=%s";

// we'll use jsonEmpty to flush AWS and to check that it's not changed between invocations
char jsonEmpty[] = "{\"myID\": \"1\",\"fbID\": \"0\", \"fbType\": \"0\", \"fbAction\": \"0\",\"fbPayload\": \"0\",\"fbComments\": \"0\", \"fbGuid\": \"0\"}";
char jsonEmptyAction[] =   "\"fbAction\": \"0\"";  // the string as presented back by AWS when we've got jsonEmpty[] back

String jsonInput;  // empty at the moment

// Use WiFiClientSecure class to create TLS connection
WiFiClientSecure client;

// web server for Fibaro or user to call
ESP8266WebServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println("");  Serial.println("................");
  Serial.print("Startup.  Version: ");  Serial.println(version);

  Serial.print (F(__DATE__)) ;
  Serial.print (" ") ;
  Serial.println (F(__TIME__)) ;

  startUpWifi(); // establish Wifi connections

  server.on("/", handleRoot);
  server.begin();

  // initial clearing of any data in AWS - so we don't pick up any odd commands in the buffer!
  clearAwsRecord(awsHost, awsPostUlr, httpsPort, jsonEmpty );

}


void loop() {

  // poll AWS periodically
  unsigned long currentMillis = millis();

  //Reserve JSON Space
  StaticJsonBuffer<500> jsonBuffer;

  blinkLed();

  server.handleClient(); // deal with any web request.
  // if the Fibaro is polling, this will be called every second or so.


  if (currentMillis - previousMillis >= interval) {
    //
    previousMillis = currentMillis;
    Serial.println("Polling AWS");

    // do this now, so that we don't have to worry about missing it if there's a
    // "return" condition

    yield();

    /// check AWS for an update
    if (checkAws() != 0) {

      // we will pull apart the JSON now to find the JSON details
      JsonObject& root = jsonBuffer.parseObject(jsonInput);
      root.printTo(Serial);
      Serial.println("");

      if (!root.success())
      {
        Serial.println("JSON parseObject() failed");
        return ;  // TODO -- need to think what we do here
      }

      const int foundFbID = root["fbID"];
      char foundFbAction[20];
      char foundFbType[20];
      char foundfbPayload[100];

      strcpy(foundFbAction, (const char*)root["fbAction"]);
      strcpy(foundFbType, (const char*)root["fbType"]);
      strcpy(foundfbPayload, (const char*)root["fbPayload"]);

      // do the Fibaro activity before clearing AWS - speed up response time to Fibaro

      postToFibaro(fibaroAddress, foundFbID, foundFbType,  foundFbAction, foundfbPayload ) ;
      clearAwsRecord(awsHost, awsPostUlr, httpsPort, jsonEmpty );
    }
  }

}

/*
   Poll AWS looking for data inthe DynamoDB record
   Return 0 - no record, is empty or Error
   Return 1 - Record has information
*/

int checkAws() {
  //  Serial.println("In checkAws()");

  if (!client.connect(awsHost, httpsPort)) {
    Serial.println("connection to AWS failed");
    return 0 ; // probably need to do something at this point
  }

  client.print(String("GET ") + awsGetUrl + " HTTP/1.1\r\n" +
               "Host: " + awsHost + "\r\n" +
               "Connection: close\r\n\r\n");
  //  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      //     Serial.println("headers received");
      break;
    }
  }

  // now read the body JSON
  while (client.connected()) {
    String webLine = client.readStringUntil('\n');
    //Serial.println(webLine);
    if (webLine == "\r") break;
    // else concatonate
    jsonInput = jsonInput + webLine ;
  }

  // the JSON format from AWS has preamble and postamble JSON is bracketed by "[" and "]" - so will be removed
  int  leftBrace =  jsonInput.indexOf('[') + 1;
  int  rightBrace = jsonInput.indexOf(']') - 1;

  jsonInput.remove(rightBrace);
  jsonInput.remove(0, leftBrace);

  if ( jsonInput.indexOf(jsonEmptyAction) == -1) // if we had some information found we need to clear AWS later.
  {
    Serial.println("Clearing AWS Record");
    return 1;
  }
  else {
    Serial.println("No AWS Activity");
    return 0;
  }

}

void handleRoot() {
  Serial.print("In HandleRoot: jsonInput: ");
  Serial.println(jsonInput);
  server.send(200, "text/plain", jsonInput);

}

/*
 ****    send up a "blank" record to replace the one that is already in DynamoDB
*/

int clearAwsRecord(char* thisHost, char* thisUrl, int thisPort, char* jsonPayload)
{

  Serial.println("In clearAwsRecord: ");

  if (!client.connect(thisHost, thisPort))
  {
    Serial.println("clearAwsRecord: connection failed");
    return 0; // probably need to do something at this point
  }

  client.println(String("POST  ") + thisUrl + String(" HTTP/1.1"));
  client.println(String("Host: ") + thisHost);
  client.println("Cache-Control: no-cache");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(strlen(jsonPayload));
  client.println();
  client.println(jsonPayload);

  long interval = 2000;
  unsigned long currentMillis = millis(), previousMillis = millis();

  /////////////////
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.print("** ");
    Serial.println(line);

    if (line == "\r") {
      Serial.println("clearAwsRecord: headers received");
      break;
    }
  }

  //  Serial.println(" ");
  //  Serial.println("clearAwsRecord:  Success");
  return 1;

}


/*
 * ***********************************************************************************
    Send the Command to Fibaro using a URL formatted as per the API configuration at
    http://YourFibaroIpAddress/docs
* ***********************************************************************************
*/

int postToFibaro(char fibaroAddress[], int deviceID, char fbType[],  char fbAction[], char fbPayload[]) {

  // fbType[] - could be "Device " or "Scene" - we'll change the URL to suit.

  char hostUrl[100];
  char fbDevicesScenes[10];

  if (strcmp(fbType , "Device") == 0) {
    sprintf(hostUrl, fbDeviceString , fibaroAddress, deviceID, fbAction, fbPayload); // copy the device URL
  }  else {
    // assuming it's a scene
    sprintf(hostUrl, fbSceneString , fibaroAddress, deviceID, fbAction, fbPayload); // copy the device URL
  };


  http.begin(hostUrl);
  http.setAuthorization(fbBase64UserName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  Serial.print("Calling URL: ");
  Serial.println(hostUrl);


  int httpCode = http.GET(); // payload is empty

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    //    Serial.printf("[HTTP] GET... Return code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      //      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] PUT... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  return httpCode;

}

/*
   WIFI starup Routines

*/

void startUpWifi() {
  Serial.print("connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// **************************************************************

void blinkLed() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousLedMillis >= ledInterval) {
    previousLedMillis = currentMillis;
    if (ledState == LOW)
      ledState = HIGH;  // Note that this switches the LED *off*
    else
      ledState = LOW;   // Note that this switches the LED *on*
    digitalWrite(LED_BUILTIN, ledState);

  }

}
