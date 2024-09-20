#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Network settings
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(10,197,12,6);

// MQTT broker settings
const char* domain = "broker.app.wnology.io";
const uint16_t port = 1883;

// device settings
const char* deviceId = "66e0e7ba1d7b3b56233a255a";
const char* user = "47d62c49-65ad-4e8b-b12c-b350548a4504";
const char* pass = "5c88f6136d960bd72f1400372306897ba6719dca31529af75077ea87cd130a74";

EthernetClient ethClient;
PubSubClient client(ethClient);

// Variables
bool currState;
String message = "";         // A string to hold incoming data
bool messageComplete = false;     // Whether the string is complete

// Turns LED on or off
void setState(bool state) {
  if(currState != state){
    if (state) {
      digitalWrite(9, 255);
    } else {
      digitalWrite(9, 0);
    }  
  }
  currState = state;
}

// Defines the brightness of the LED
void setIntensity(uint16_t intensity) {
  if(currState){
    analogWrite(9, intensity);
  }
}

// Read and print message
void receiveMessage(char *topic, char *message){
  if(strcmp(topic,"message/platform") == 0){
    Serial.println("Platform: " + message);
  }else if(strcmp(topic,"message") == 0){
    Serial.println("Explorer: " + message);
  }else{
    Serial.println("Unknown: " + message);
  }
}

// Sends a message to the IoT Platform
void sendMessage(){
  if (messageComplete) {
    Serial.println("You: "+ message);

    client.publish("message", message.c_str());

    message = "";
    messageComplete = false;
  }
}

// Called whenever I type a letter in the Serial Monitor
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();

    message += inChar;

    if (inChar == '\n') {
      messageComplete = true;
    }
  }
}

// Callback function to handle incoming messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  // Convert payload to string
  String payloadStr;
  for (unsigned int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }
  Serial.println(payloadStr);
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(deviceId, user, pass)) {
      Serial.println("connected");
      client.subscribe("message");
      client.subscribe("message/platform");
      client.subscribe("led");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


// --------------- RUN THE APPLICATION ---------------

void setup()
{
  Serial.begin(9600);
  inputString.reserve(200);
  pinMode(9, OUTPUT);

  client.setServer(domain, port);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);

}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
