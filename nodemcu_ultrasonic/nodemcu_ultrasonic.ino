#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>

// WiFi credentials
const char* ssid = "SAYANI_WIFI";
const char* password = "00011101";

// MQTT broker credentials
const char* mqtt_server = "192.168.31.10";
const int mqtt_port = 1883;
const char* mqtt_topic_water = "home/tank/water";
const char* mqtt_topic_light = "home/environment/light";

// Define the pins used to connect the JSN-SR04T sensor to the Arduino
const int TRIG_PIN = D1;
const int ECHO_PIN = D2;

// LDR sensor pin
const int ldr_sensor_pin = A0;

// Define the interval in milliseconds to read the LDR sensor data
const int interval = 10000;

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
    // Create a random client ID
    String clientId = "ESP8266-";
    clientId += String(random(0xffff), HEX);
    while (!mqttClient.connect(clientId.c_str())) {
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

  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  // Convert the distance reading to a string
  sprintf(readingCharArray, "%d", distance);

   // Publish the distance to the MQTT topic
  mqttClient.publish(mqtt_topic_water, readingCharArray);
}

// Publish LDR sensor value to MQTT topic
void publishBrightness() {
    // Read LDR sensor value
    int ldr_sensor_value = analogRead(ldr_sensor_pin);

    // Print the sensor value to the serial monitor
    Serial.print("Brightness: ");
    Serial.println(ldr_sensor_value);

    // Convert LDR sensor value to string
    sprintf(readingCharArray, "%d", ldr_sensor_value);

    // Publish LDR sensor value to MQTT topic
    mqttClient.publish(mqtt_topic_light, readingCharArray);
}

// Setup function
void setup() {
  // Set the LED pin as output
  pinMode(LED_BUILTIN, OUTPUT);

  // Set LDR sensor pin as input
  pinMode(ldr_sensor_pin, INPUT);

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

  // Publish the light to the MQTT topic
  publishBrightness();

  // Blink the LED
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);

  // Wait for a second before measuring the distance again
  delay(1000);
}
