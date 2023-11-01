#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>

// WiFi credentials
const char* ssid = "SAYANI_WIFI";
const char* password = "00011101";

// MQTT broker credentials
const char* mqtt_server = "192.168.8.10";
const int mqtt_port = 1883;
const char* mqtt_topic = "home/terrace/tank/water/level";

// Define the pins used to connect the JSN-SR04T sensor to the Arduino
const int TRIG_PIN = D1;
const int ECHO_PIN = D2;

// Create an instance of the Ultrasonic class
Ultrasonic ultrasonic(TRIG_PIN, ECHO_PIN);

// Create a char array to store the converted reading
char readingCharArray[10];

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Reconnect to WiFi and MQTT broker
void reconnect() {
  if (!WiFi.isConnected()) {
    Serial.println("Reconnecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to the WiFi network");
  }

  if (!mqttClient.connected()) {
    Serial.println("Reconnecting to MQTT broker...");
    while (!mqttClient.connect("ESP8266")) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to the MQTT broker");
  }
}

// Publish the distance to the MQTT topic
void publishDistance() {
  // Get the distance from the ultrasonic sensor
  long distance = ultrasonic.read();

  // Blink the LED
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);

  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  // Convert the distance reading to a string
  sprintf(readingCharArray, "%d", distance);

   // Publish the distance to the MQTT topic
  //mqttClient.publish(mqtt_topic, String(distance).c_str());
  mqttClient.publish(mqtt_topic, readingCharArray);
}

// Setup function
void setup() {
  // Set the LED pin as output
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  mqttClient.setServer(mqtt_server, mqtt_port);  
}

// Loop function
void loop() {
  // Reconnect to WiFi and MQTT broker if necessary
  reconnect();

  // Keep the MQTT connection alive
  mqttClient.loop();

  // Publish the distance to the MQTT topic
  publishDistance();

  // Wait for a second before measuring the distance again
  delay(1000);
}
