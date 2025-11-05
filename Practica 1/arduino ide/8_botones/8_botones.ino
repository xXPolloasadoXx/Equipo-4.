#if defined(ESP32)
#include <WiFi.h>
#elif defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#endif
#include <PubSubClient.h>


const char* ssid = "Megcable_2.4G_68BE";
const char* password = "ftuhtdGE";
const char* mqtt_server = "test.mosquitto.org";

/*const char* ssid = "InstVirtual";
const char* password = "37418601";
const char* mqtt_server = "192.168.0.102";*/

// Configuración de pines
const int botones[] = { 17, 18, 19, 21, 22, 23, 26, 25 };
const int numBotones = 8;
const char* topics[] = {
  "7c/eq4/bt1", "7c/eq4/bt2", "7c/eq4/bt3", "7c/eq4/bt4",
  "7c/eq4/bt5", "7c/eq4/bt6", "7c/eq4/bt7", "7c/eq4/bt8"
};

// Variables de estado
long lastMsg = 0;
int estadoAnterior[numBotones] = { -1 };  // -1 para forzar primera publicación
bool estadosBotones[numBotones];

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // Configurar pines de entrada
  for (int i = 0; i < numBotones; i++) {
    pinMode(botones[i], INPUT);
  }

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  Serial.print("\nConectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nError: No se pudo conectar al WiFi");
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  String msgTemp;
  for (int i = 0; i < length; i++) {
    msgTemp += (char)message[i];
  }
  // Aquí puedes agregar lógica para comandos entrantes si es necesario
}

void reconnect() {
  int intentos = 0;
  while (!client.connected() && intentos < 5) {
    Serial.print("\nReconectando servidor MQTT...");
    if (client.connect("ESP32_2_eq4_8botones")) {
      Serial.println("conectado");
      break;
    } else {
      Serial.print("Fallo, rc = ");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5 segundos...");
      delay(5000);
      intentos++;
    }
  }
}

void publicarEstadoBotones() {
  bool cambioDetectado = false;

  // Leer todos los botones primero
  for (int i = 0; i < numBotones; i++) {
    estadosBotones[i] = digitalRead(botones[i]);

    // Solo publicar si hubo cambio
    if (estadosBotones[i] != estadoAnterior[i]) {
      cambioDetectado = true;
      estadoAnterior[i] = estadosBotones[i];
    }
  }

  // Publicar solo si hubo cambios
  if (cambioDetectado) {
    for (int i = 0; i < numBotones; i++) {
      char mensaje[2];
      sprintf(mensaje, "%d", estadosBotones[i]);

      if (client.publish(topics[i], mensaje)) {
        Serial.printf("Publicado: %s -> %s\n", topics[i], mensaje);
      } else {
        Serial.printf("Error publicando en: %s\n", topics[i]);
      }

      // Pequeña pausa entre publicaciones
      delay(10);
    }
    Serial.println("---");  // Separador para mejor legibilidad
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  long varNow = millis();
  if (varNow - lastMsg >= 500) {
    publicarEstadoBotones();
    lastMsg = varNow;
  }

  client.loop();
}