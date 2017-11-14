/*
 Basic ESP8266 MQTT light switch

 This sketch uses the pubsub and RCSwitch library in combination
 with the ESP8266 board/library & 315Mhz transmittor 

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - Uses case to determine action based on the first character of the topic "inTopic"
    case 0, switch OFF the ESP Led,
    case 1, switch ON the ESP Led,
    A-F switch corresponding remote keys
  

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// stuff for RC
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();


// Update these with values suitable for your network.
const char* mqtt_server = "192.168.1.240";
const char* mqtt_out_topic = "565_sub";
const char* mqtt_in_topic = "565_pub";
const char* mqtt_username = "domo";
const char* mqtt_password = "zondag";

WiFiClient espClient;
const char* ssid = " your ssid here";
const char* wifi_password = " your pw here";

String clientId = "RCSWITCH-565";

PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(mqtt_out_topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

//  // Switch on the LED if an 1 was received as first character
//  if ((char)payload[0] == '1') {
//    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
//    // but actually the LED is on; this is because
//    // it is acive low on the ESP-01)
//  } else {
//    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
//  }

 
  switch(payload[0])
  {
    case '1':
      digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
      break;
    case '0':
      digitalWrite(BUILTIN_LED, HIGH);   // Turn the LED on (Note that LOW is the voltage level
      break;
    case 'A':
      mySwitch.send("010000010011011111000000");
      delay(1000);
      break;
    case 'B':
      mySwitch.send("010000010011011100110000");
      delay(1000);
      break; 
    case 'C':
      mySwitch.send("010000010011011111000000");
      delay(1000);   
      break;
    case 'D':
      mySwitch.send("010000010011011100000000");
      delay(1000); 
      break;
    case 'E':
      mySwitch.send("010000010011011100001111");
      delay(1000);  
      break;
    case 'F':
      mySwitch.send("010000010011011111110000");
      delay(1000);  
      break;
    

    default:
      for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);}
      Serial.print(" was received, but not expected.");
      break;

  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
//    String clientId = "ESP8266Client-";
//    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_out_topic, "hello world");
      // ... and resubscribe
      client.subscribe(mqtt_in_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

   mySwitch.enableTransmit(0);  // Optional set pulse length.
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);
//    client.publish(mqtt_out_topic, msg);
  }
}
