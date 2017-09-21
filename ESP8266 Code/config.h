/*
   Build-specific variables and constants
*/

const char version[] = "Version Number Here";

// WIFI details
char ssid[]     = "mySSID";
char password[] = "myWifiPassword";


// ****** AWS Details ******
//   Setting up AWS - https://obviate.io/2015/08/05/tutorial-aws-api-gateway-to-lambda-to-dynamodb/
char awsHost[]  = "YourInfo.execute-api.AmazonRegion.amazonaws.com";
char awsGetUrl[]  = "/LambdaPublished/getitem/1";  // ** If you're selecting record ID 1
char awsPostUlr[]  = "/LambdaPublished/submitghdata";
int httpsPort = 443;


char awsGuid[]  = "StickInAGuid Here"; // could be anything - byt https://guidgenerator.com/online-guid-generator.aspx is a good start

// ****** Fibaro Details ******
char* fibaroAddress  = "YourFibaroIpAddress";
char* fbBase64UserName  = "Base64UsernamePassword";
/*
    fibaro username:password as base64 
    DON'T use your Admin Password - create another one in the console
    Use https://www.base64encode.org/ to create encoded version
*/
