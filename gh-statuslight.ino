#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <aJSON.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0xDA, 0xEA, 0xFD, 0xDF, 0xCE, 0xDC };
char server[] = "thewcubed.com";

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

boolean firstCurly = true;
boolean startRead = false;
String jsonString = "";

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 30*1000;  // delay between updates, in milliseconds

void setup() {
	DDRD = DDRD | B11111100; //sets pins 2 to 7 as outputs

	//Initialize serial
	Serial.begin(9600);

	//Start the Ethernet up
	while (Ethernet.begin(mac) != 1)
		{
			Serial.println("Error getting IP address via DHCP, trying again...");
			delay(15000);
	}
	//print the Ethernet board/shield's IP address:
	Serial.print("My IP address: ");
	Serial.println(Ethernet.localIP());
}

void loop()
{
	if (client.available()) {
		char c = client.read();
		if( firstCurly == true && c == '{' ) { startRead = true; firstCurly = false; }; // signals beginning of JSON
		if( c == ',' && startRead == true ) { c = '}';};
		if( startRead ) { Serial.print(c); jsonString += c; };
		if( c == '}' && startRead == true ) { startRead = false; };
	}
	
	// if there's no net connection, but there was one last time
	// through the loop, then let's stop the client and parse some json
	if (!client.connected() && lastConnected) {
		Serial.println();
		Serial.println("disconnecting.");
		client.stop();
		firstCurly = true;

		Serial.println("");
		Serial.println("Debug:");
		//Serial.println("JSON string:");
		//Serial.println(jsonString);

		char charArray[jsonString.length()+1];
		jsonString.toCharArray(charArray,jsonString.length()+1);
		//Serial.println("");
		//Serial.println("Char Array:");
		//Serial.println(charArray);
		parseStatus(charArray);
	}

	// if you're not connected, and ten seconds have passed since
	// your last connection, then connect again and send data:
	if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
		jsonString = "";
		httpRequest();
	}
	// store the state of the connection for next time through
	// the loop:
	lastConnected = client.connected();

}

void parseStatus(char *json){
	aJsonObject* jsonObject = aJson.parse(json);
	aJsonObject* status = aJson.getObjectItem(jsonObject, "status");
 
	String statusString = String(status->valuestring);
	Serial.println("");
	Serial.println("Status String:");
	Serial.println(statusString);
	
	if (statusString == "good"){
		Serial.println("Green");
		PORTD = B00001000; // sets digital pin 3 HIGH
	} else if (statusString == "minorproblem") {
		Serial.println("Orange");
		PORTD = B00010000; // sets digital pin 4 HIGH
	} else if (statusString == "majorproblem"){
		Serial.println("Red");
		PORTD = B00100000; // sets digital pin 5 HIGH
	} else {
		Serial.println("Whoa there");
	}
}

// this method makes a HTTP connection to the server:
void httpRequest() {
	// if there's a successful connection:
	if (client.connect(server, 80)) {
		Serial.println("");
		Serial.println("connecting...");
		// send the HTTP PUT request:
		client.println("GET /statusmirror.php HTTP/1.1");
		client.println("Host: thewcubed.com");
		client.println("User-Agent: arduino-ethernet");
		client.println("Connection: close");
		client.println();

		// note the time that the connection was made:
		lastConnectionTime = millis();
	} 
	else {
		// if you couldn't make a connection:
		Serial.println("connection failed");
		Serial.println("disconnecting.");
		client.stop();
	}
}



