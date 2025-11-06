#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ARDUINO_UNOWIFIR4)
  #include <WiFiS3.h>
#endif
#include <PubSubClient.h>

const char* ssid = "Megcable_2.4G_68BE";
const char* password = "ftuhtdGE";
const char* mqtt_server = "test.mosquitto.org";

// Variables de tiempo
unsigned long lastMsg = 0;
const unsigned long PUBLISH_INTERVAL = 50; // 300ms entre publicaciones

// Variables para filtrado
int valPot1 = 0, valPot2 = 0, valPot3 = 0, valPot4 = 0;
int lastSent1 = -1, lastSent2 = -1, lastSent3 = -1, lastSent4 = -1;
const int FILTER_SAMPLES = 5; // Número de muestras para promedio
const int CHANGE_THRESHOLD = 15; // Cambio mínimo para publicar

// Buffers para filtrado
int buffer1[FILTER_SAMPLES], buffer2[FILTER_SAMPLES], buffer3[FILTER_SAMPLES], buffer4[FILTER_SAMPLES];
int bufferIndex = 0;

char msg[20];

WiFiClient espClient;
PubSubClient client(espClient);

void setup(){
  Serial.begin(115200);
  pinMode(32, INPUT);
  pinMode(36, INPUT);
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  
  // Inicializar buffers
  for(int i = 0; i < FILTER_SAMPLES; i++) {
    buffer1[i] = analogRead(32);
    buffer2[i] = analogRead(36);
    buffer3[i] = analogRead(34);
    buffer4[i] = analogRead(35);
  }
  
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

void callback(char* topic, byte* message, unsigned int length){
  String msgTemp;
  for(int i = 0; i < length; i++){
    msgTemp += (char)message[i];
  }
}

void reconnect(){
  while(!client.connected()){
    Serial.print("\nReconectando servidor MQTT...");   
    if(client.connect("ESP32_7ceq4_pwm")){
      Serial.println("conectado");
    }else{
      Serial.print("Fallo, rc = ");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5 segundos...");
      delay(5000);
    }
  }
}

// Función para filtrado por promedio
int filteredRead(int pin, int* buffer) {
  // Actualizar buffer
  buffer[bufferIndex] = analogRead(pin);
  
  // Calcular promedio
  long sum = 0;
  for(int i = 0; i < FILTER_SAMPLES; i++) {
    sum += buffer[i];
  }
  return sum / FILTER_SAMPLES;
}

// Función para publicar solo si hay cambio significativo
void publishIfChanged(const char* topic, int currentValue, int& lastSentValue) {
  if(abs(currentValue - lastSentValue) > CHANGE_THRESHOLD) {
    sprintf(msg, "%d", currentValue);
    if(client.publish(topic, msg)) {
      lastSentValue = currentValue;
      Serial.printf("📤 %s: %d\n", topic, currentValue);
    }
    delay(10); // Pequeña pausa entre publicaciones
  }
}

void loop(){
  if(!client.connected()){
    reconnect();
  }
  
  unsigned long currentTime = millis();
  if(currentTime - lastMsg > PUBLISH_INTERVAL){
    lastMsg = currentTime;
    
    // Leer y filtrar todos los potenciómetros
    valPot1 = filteredRead(32, buffer1);
    valPot2 = filteredRead(36, buffer2);
    valPot3 = filteredRead(34, buffer3);
    valPot4 = filteredRead(35, buffer4);
    
    // Actualizar índice del buffer
    bufferIndex = (bufferIndex + 1) % FILTER_SAMPLES;
    
    // Publicar solo si hay cambios significativos
    publishIfChanged("7c/eq4/adc1", valPot1, lastSent1);
    publishIfChanged("7c/eq4/adc2", valPot2, lastSent2);
    publishIfChanged("7c/eq4/adc3", valPot3, lastSent3);
    publishIfChanged("7c/eq4/adc4", valPot4, lastSent4);
    
    // Debug opcional
    Serial.printf("💡 ADC Filtrados: %d, %d, %d, %d\n", valPot1, valPot2, valPot3, valPot4);
  }
  
  client.loop();
}