
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <LcdBarGraphX.h>

const char* ssid = "SAYANI_IOT";
const char* password = "00001111";
const char* mqtt_server = "192.168.8.10";

WiFiClient espClient;
PubSubClient client(espClient);

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
LcdBarGraphX lbg(&lcd, 16, 0, 0);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  lcd.clear();
  lcd.print("WiFi...");
  lcd.blink();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.noBlink();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String str = String((char *) payload);
  int index = str.indexOf(',');
  
  //lcd.clear();
  if(index != -1) {
    
    //lcd.print(str.substring(0,index));
    //lcd.setCursor(0,1);
    lbg.drawValue(str.substring(0,index).toInt(), 100);

    lcd.setCursor(0,1);
    lcd.print(str.substring(index + 1));
       
  } else {
    lcd.print((char *)payload);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    lcd.clear();
    lcd.print("MQTT...");
    lcd.blink();
    
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("home/water/level");
      lcd.clear();
      lcd.print("Waiting...");
      lcd.noBlink();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  
  // initialize the LCD
  lcd.begin();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Turn on the blacklight and print a message.
  //lcd.noBacklight();
  //lbg.drawValue(10, 100);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
