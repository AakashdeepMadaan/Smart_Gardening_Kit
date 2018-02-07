// C code which was running on Intel Galileo Gen2

#include <SPI.h>
#include <Ethernet.h>
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:

int temp,v,v1;
long duration,distance;
#define trigPin 13
#define echoPin 12

byte mac[] = {
0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress ip(192,168,0,225);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):

EthernetServer server(80);

void setup() {
	// Open serial communications and wait for port to open:
	Serial.begin(9600);
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
	pinMode(3, OUTPUT);
	while (!Serial) {
		; // wait for serial port to connect. Needed for Leonardo only
	}
	// start the Ethernet connection and the server:
	Ethernet.begin(mac, ip);
	server.begin();
	Serial.print("server is at ");
	Serial.println(Ethernet.localIP());
}
void loop() {
	// listen for incoming clients
	EthernetClient client = server.available();
	if (client) {
		Serial.println("new client");
		// an http request ends with a blank line
		boolean currentLineIsBlank = true;
		while (client.connected()) {
			if (client.available()) {
				char c = client.read();
				Serial.write(c);
				// if you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so you can send a reply
				if (c == '\n' && currentLineIsBlank) {
					// send a standard http response header
					client.println("HTTP/1.1 200 OK");
					client.println("Content-Type: text/html");
					client.println("Connection: close");
					client.println();
					client.println("<!DOCTYPE HTML>");
					client.println("<html>");
					// add a meta refresh tag, so the browser pulls again every 5 seconds:
					client.println("<meta http-equiv=\"refresh\" content=\"5\">");
					// output the value of each analog input pin
					delay(1000);
					int temp = analogRead(0);
					v=(4.8*temp)/10;
					v1=370-v;
					client.print("temperature is ");
					client.print(v1);
					client.println("<br />");
					digitalWrite(trigPin, LOW); // Added this line
					delayMicroseconds(2); // Added this line
					digitalWrite(trigPin, HIGH);
					// delayMicroseconds(1000); - Removed this line
					delayMicroseconds(10); // Added this line
					digitalWrite(trigPin, LOW);
					duration = pulseIn(echoPin, HIGH);
					distance = (duration/2) / 29.1;
					client.print("Distance is ");
					client.print(distance);
					client.print("cm ");
					client.println("<br />");
					client.print("Moisture level ");
				
					int moist = analogRead(1);
					if(moist>300 && moist<700)
						client.print("NORMAL ");
					if(moist<300)
						client.print("OVER IRRIGATION ");
					if(moist>700) {
						client.print("UNDER IRRIGATION ");
						digitalWrite(3,1);
					}
					client.println("</html>");
					break;
				}
				if (c == '\n') {
					// you're starting a new line
					currentLineIsBlank = true;
				}
				else if (c != '\r') {
					// you've gotten a character on the current line
					currentLineIsBlank = false;
				}
			}
		}
		// give the web browser time to receive the data
		delay(1);
		// close the connection:
		client.stop();
		Serial.println("client disonnected");
	}
}
