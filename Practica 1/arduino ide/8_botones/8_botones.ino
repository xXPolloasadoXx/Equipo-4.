// Modificar SSID y PASSWORD antes de grabar el ESP32

#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "InstVirtual";           // Cambiar por el nombre de tu red
const char* password = "37418601";          // Cambiar por tu contraseña

const char* mqtt_server = "192.168.0.102";  // Servidor MQTT local

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  
  pinMode(17, INPUT);    // Botón 1
  pinMode(18, INPUT);    // Botón 2
  pinMode(19, INPUT);    // Botón 3
  pinMode(21, INPUT);    // Botón 4
  pinMode(22, INPUT);    // Botón 5
  pinMode(23, INPUT);    // Botón 6
  pinMode(26, INPUT);    // Botón 7
  pinMode(25, INPUT);    // Botón 8
  
  Serial.println("Todos los botones configurados");
  
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  // Configurar MQTT
  client.setServer(mqtt_server, 1883);
}

void conectar_mqtt() {
  while (!client.connected()) {
    Serial.println("Conectando al servidor MQTT...");
    
    if (client.connect("ESP32_2_eq4_8botones")) {
      Serial.println("Conectado a MQTT!");
    } else {
      Serial.println("Error de conexión, reintentando...");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    conectar_mqtt();
  }
  
  // Botón 1
  int estado1 = digitalRead(17);
  if (estado1 == 1) {
    client.publish("7c/eq4/bt1", "1");
  } else {
    client.publish("7c/eq4/bt1", "0");
  }
  delay(10);
  
  // Botón 2
  int estado2 = digitalRead(18);
  if (estado2 == 1) {
    client.publish("7c/eq4/bt2", "1");
  } else {
    client.publish("7c/eq4/bt2", "0");
  }
  delay(10);
  
  // Botón 3
  int estado3 = digitalRead(19);
  if (estado3 == 1) {
    client.publish("7c/eq4/bt3", "1");
  } else {
    client.publish("7c/eq4/bt3", "0");
  }
  delay(10);
  
  // Botón 4
  int estado4 = digitalRead(21);
  if (estado4 == 1) {
    client.publish("7c/eq4/bt4", "1");
  } else {
    client.publish("7c/eq4/bt4", "0");
  }
  delay(10);
  
  // Botón 5
  int estado5 = digitalRead(22);
  if (estado5 == 1) {
    client.publish("7c/eq4/bt5", "1");
  } else {
    client.publish("7c/eq4/bt5", "0");
  }
  delay(10);
  
  // Botón 6
  int estado6 = digitalRead(23);
  if (estado6 == 1) {
    client.publish("7c/eq4/bt6", "1");
  } else {
    client.publish("7c/eq4/bt6", "0");
  }
  delay(10);
  
  // Botón 7
  int estado7 = digitalRead(26);
  if (estado7 == 1) {
    client.publish("7c/eq4/bt7", "1");
  } else {
    client.publish("7c/eq4/bt7", "0");
  }
  delay(10);
  
  // Botón 8
  int estado8 = digitalRead(25);
  if (estado8 == 1) {
    client.publish("7c/eq4/bt8", "1");
  } else {
    client.publish("7c/eq4/bt8", "0");
  } 
  delay(500); 
  client.loop(); 
}