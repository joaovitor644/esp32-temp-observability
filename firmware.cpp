#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ====== Configurações MQTT ======
const char* MQTT_BROKER = "18.216.60.2"; // Broker público para testes no Wokwi
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "esp32c6/observability/sensor1";

// ====== Credenciais Wi-Fi (Padrão Wokwi) ======
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";

WiFiClient espClient;
PubSubClient mqttClient(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pinoPot = 1;  
const int pinoNTC = 0;  
const float BETA = 3950; 

unsigned long lastPublish = 0;

void setup() {
  Serial.begin(115200);
  
  // LCD e I2C (SDA=6, SCL=7)
  Wire.begin(6, 7); 
  lcd.init();
  lcd.backlight();
  lcd.print("Conectando...");

  // Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Conectado!");
  
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Tentando MQTT...");
    // Criando um ID único para evitar conflitos no broker
    String clientId = "ESP32C6-Client-" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("Conectado!");
    } else {
      delay(2000);
    }
  }
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop(); // Essencial para manter a conexão ativa

  // Leitura Sensores
  int valorNTC = analogRead(pinoNTC);
  float celsius = 1 / (log(1 / (4095.0 / valorNTC - 1)) / BETA + 1.0 / 298.15) - 273.15;
  
  int valorPot = analogRead(pinoPot);
  float ganho = 1.0 + (valorPot / 4095.0) * 9.0; 
  float tempCalculada = celsius * ganho;

  // Atualização do LCD (mais rápida que o MQTT)
  static unsigned long lastLCD = 0;
  if (millis() - lastLCD > 500) {
    lcd.setCursor(0, 0);
    lcd.print("T:"); lcd.print(celsius, 1);
    lcd.print("C G:"); lcd.print(ganho, 1);
    lcd.setCursor(0, 1);
    lcd.print("Final: "); lcd.print(tempCalculada, 1);
    lcd.print("C   ");
    lastLCD = millis();
  }

  // Publicação MQTT (a cada 2 segundos para não sobrecarregar)
  if (millis() - lastPublish > 2000) {
    String payload = "{\"real_temp\":" + String(celsius, 2) + 
                     ",\"ganho\":" + String(ganho, 2) + 
                     ",\"total\":" + String(tempCalculada, 2) + "}";
    
    mqttClient.publish(MQTT_TOPIC, payload.c_str());
    Serial.println("Dados publicados: " + payload);
    lastPublish = millis();
  }
}