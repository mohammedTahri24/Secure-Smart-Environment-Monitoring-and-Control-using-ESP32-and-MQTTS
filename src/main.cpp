#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <ESP32Servo.h>

#define DHTPIN 14
#define LED_PIN 25
#define BUZZER_PIN 19
#define SERVO_PIN 18
#define BUTTON1_PIN 4
#define BUTTON2_PIN 2

// WiFi et MQTT
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

// Topics MQTT
const char* DEVICE_ID = "esp32-01";
char topic_tel[50];
char topic_cmd[50];
char topic_evt[50];

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHT22);
Servo servo;

// Variables
float temp = 0;
float hum = 0;

bool alerte = false;
bool fan = false;
bool mute = false;

unsigned long lastMsg = 0;
unsigned long lastBuzz = 0;
bool buzzState = false;

// Plages normales
const float TEMP_MIN = 10.0;
const float TEMP_MAX = 37.0;
const float HUM_MIN = 40.0;
const float HUM_MAX = 60.0;

void setup_wifi() {
  Serial.print("Connexion au WiFi...");
  WiFi.begin(ssid, password);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" OK");
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(" ÉCHEC");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  char msg[256];

  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  msg[length] = '\0';

  StaticJsonDocument<256> doc;
  deserializeJson(doc, msg);

  const char* cmd = doc["cmd"];

  if (strcmp(cmd, "SERVO_SET") == 0) {
    bool val = doc["value"];

    if (val) {
      fan = true;
      servo.write(90);
      Serial.println("Ventilateur ACTIVÉ");
    } else {
      fan = false;
      servo.write(0);
      Serial.println("Ventilateur DÉSACTIVÉ");
    }
  }

  else if (strcmp(cmd, "ALARM_MUTE") == 0) {
    mute = doc["value"];

    if (mute) {
      digitalWrite(BUZZER_PIN, LOW);
      buzzState = false;
    }

    Serial.print("Mode muet : ");
    Serial.println(mute ? "ACTIVÉ" : "DÉSACTIVÉ");
  }

  else if (strcmp(cmd, "ALERT_RESET") == 0) {
    alerte = false;
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    buzzState = false;

    Serial.println("Alerte réinitialisée");
  }
}

void reconnect() {
  int tries = 0;

  while (!client.connected() && tries < 3) {
    Serial.print("Connexion au broker MQTT...");

    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println(" OK");
      client.subscribe(topic_cmd);
    } else {
      Serial.print(" Échec, code = ");
      Serial.println(client.state());
      tries++;
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== DÉMARRAGE DU SYSTÈME IoT ===");

  // Création des topics MQTT
  sprintf(topic_tel, "warehouse/%s/telemetry", DEVICE_ID);
  sprintf(topic_cmd, "warehouse/%s/commands", DEVICE_ID);
  sprintf(topic_evt, "warehouse/%s/events", DEVICE_ID);

  // Configuration des broches
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Servo moteur
  servo.attach(SERVO_PIN);
  servo.write(0);

  // Capteur DHT
  dht.begin();

  // Connexion WiFi
  setup_wifi();

  

//espClient.setInsecure();
  // Connexion MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.println("Configuration terminée !");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  unsigned long now = millis();

  // Lecture du capteur toutes les 5 secondes
  if (now - lastMsg > 5000) {
    lastMsg = now;

    hum = dht.readHumidity();
    temp = dht.readTemperature();

    if (!isnan(temp) && !isnan(hum)) {

      // Vérification des plages normales
      bool temperatureAnormale = (temp < TEMP_MIN || temp > TEMP_MAX);
      bool humiditeAnormale = (hum < HUM_MIN || hum > HUM_MAX);

      bool conditionAlerte = temperatureAnormale || humiditeAnormale;

      // Activation de l'alerte
      if (conditionAlerte && !alerte) {
        alerte = true;
        digitalWrite(LED_PIN, HIGH);

        Serial.println("*** ALERTE ACTIVÉE ***");

        if (temperatureAnormale) {
          Serial.println("Température hors de la plage normale !");
        }

        if (humiditeAnormale) {
          Serial.println("Humidité hors de la plage normale !");
        }

        // Événement MQTT
        StaticJsonDocument<128> evt;
        evt["deviceId"] = DEVICE_ID;
        evt["type"] = "ENV_ALERT";
        evt["tempC"] = temp;
        evt["hum"] = hum;

        char evtOut[128];
        serializeJson(evt, evtOut);
        client.publish(topic_evt, evtOut);
      }

      // Désactivation de l'alerte si tout redevient normal
      if (!conditionAlerte && alerte) {
        alerte = false;
        digitalWrite(LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
        buzzState = false;

        Serial.println("Température et humidité normales");
      }

      // Télémétrie MQTT
      // IMPORTANT : les noms JSON sont gardés comme avant pour Node-RED
      StaticJsonDocument<192> tel;
      tel["deviceId"] = DEVICE_ID;
      tel["tempC"] = (int)(temp * 10) / 10.0;
      tel["hum"] = (int)(hum * 10) / 10.0;
      tel["alert"] = alerte;
      tel["fan"] = fan;
      tel["muted"] = mute;

      char telOut[192];
      serializeJson(tel, telOut);
      client.publish(topic_tel, telOut);

      Serial.printf(
        "Température : %.1f °C | Humidité : %.1f %% | Alerte : %d | Ventilateur : %d\n",
        temp,
        hum,
        alerte,
        fan
      );
    }
  }

  // Gestion du buzzer
  if (alerte && !mute) {
    if (now - lastBuzz > 500) {
      lastBuzz = now;
      buzzState = !buzzState;
      digitalWrite(BUZZER_PIN, buzzState);
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    buzzState = false;
  }

  // Boutons
  static unsigned long lastBtn1 = 0;
  static unsigned long lastBtn2 = 0;

  // Bouton 1 : activer/désactiver le ventilateur
  if (digitalRead(BUTTON1_PIN) == HIGH && (now - lastBtn1 > 300)) {
    lastBtn1 = now;

    fan = !fan;
    servo.write(fan ? 90 : 0);

    Serial.print("Ventilateur : ");
    Serial.println(fan ? "ACTIVÉ" : "DÉSACTIVÉ");
  }

  // Bouton 2 : activer/désactiver le mode muet
  if (digitalRead(BUTTON2_PIN) == LOW && (now - lastBtn2 > 300)) {
    lastBtn2 = now;

    mute = !mute;

    if (mute) {
      digitalWrite(BUZZER_PIN, LOW);
      buzzState = false;
    }

    Serial.print("Mode muet : ");
    Serial.println(mute ? "ACTIVÉ" : "DÉSACTIVÉ");
  }

  delay(50);
}