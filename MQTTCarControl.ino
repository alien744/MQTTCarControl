#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//定義馬達
#define D1 5
#define D2 4
#define D3 0
#define D4 2
//小車連接WiFi的設定
#define STASSID "DaBear"
#define STAPSK  "100godbl"

const char* ssid     = STASSID;
const char* password = STAPSK;
//小車速度設定
int velocity = 200;
//broker位置、MQTT資訊 +S
IPAddress server(140, 127, 196, 39);
#define MQTT_PORT 18883
const char mqtt_user[] = "tinyml";
const char mqtt_pass[] = "isuCSIE2021#";
WiFiClient wifi_client;
PubSubClient client(wifi_client);

// Functions for controlling wifi car 小車控制
void forward()
{
  analogWrite(D1, velocity);
  analogWrite(D2, velocity);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, HIGH);
}

void stop()
{
  analogWrite(D1, 0);
  analogWrite(D2, 0);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, HIGH);
}

void left()
{
  analogWrite(D1, velocity);
  analogWrite(D2, velocity);
  digitalWrite(D3, LOW);
  digitalWrite(D4, HIGH);
}

void right()
{
  analogWrite(D1, velocity);
  analogWrite(D2, velocity);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, LOW);
}

// Functions for MQTT operation MQTT操作小車 +S

// "go" --> "1" go 設定成傳送1到小車 類推 +S
// "stop" --> "2" +S
// "left" --> "3" +S
// "right" --> "4" +S
void callback(char* topic, byte* payload, unsigned int length) {
  // output debug message
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // control wificar by payload command 接收MQTT指令控制小車
  int cmd;
  cmd = payload[0] - '0';
  switch (cmd) {
    case 1:
      forward();
    break;
    case 2:
      stop();
    break;
    case 3:
      left();
    break;
    case 4:
      right();
    break;
    default: {}
  }
}
//重新連線
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("wifiCar", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("wificar","WiFi Car Ready ...");
      // ... and resubscribe
      client.subscribe("tinyml");
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Main Program
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  Serial.println("hello");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  client.setServer(server, MQTT_PORT);
  client.setCallback(callback);

  Serial.println("wificar ready!");
}

void loop() {
  if (!client.connected()) {
    digitalWrite(LED_BUILTIN, LOW);
    reconnect();
  }
  client.loop();
}
