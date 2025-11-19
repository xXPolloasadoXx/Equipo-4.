#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "InstVirtual";
const char* password = "37418601";
const char* mqtt_server = "192.168.0.102";

long lastMsg = 0;
int valPot1 = 0, valPot2 = 0, valPot3 = 0, valPot4 = 0;
char msg[20];

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  
  pinMode(32, INPUT);
  pinMode(36, INPUT);
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi(){
  Serial.print("\nConectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("\nReconectando servidor MQTT...");
    if (client.connect("ESP32_7ceq4")) {
      Serial.println("conectado");
    } else {
      Serial.print("Fallo, rc = ");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5 segundos...");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  long now = millis();
  if (now - lastMsg > 800) {
    lastMsg = now;
    
    // Leer los potenciómetros
    valPot1 = analogRead(32);
    valPot2 = analogRead(36);
    valPot3 = analogRead(34);
    valPot4 = analogRead(35);
    
    // Publicar todos los valores
    sprintf(msg, "%d", valPot1);
    client.publish("7c/eq4/adc1", msg);
    
    sprintf(msg, "%d", valPot2);
    client.publish("7c/eq4/adc2", msg);
    
    sprintf(msg, "%d", valPot3);
    client.publish("7c/eq4/adc3", msg);
    
    sprintf(msg, "%d", valPot4);
    client.publish("7c/eq4/adc4", msg);
    
    // Mostrar valores en el monitor serial
    Serial.print("ADC1: ");
    Serial.print(valPot1);
    Serial.print(" ADC2: ");
    Serial.print(valPot2);
    Serial.print(" ADC3: ");
    Serial.print(valPot3);
    Serial.print(" ADC4: ");
    Serial.println(valPot4);
  }
  
  client.loop();
}