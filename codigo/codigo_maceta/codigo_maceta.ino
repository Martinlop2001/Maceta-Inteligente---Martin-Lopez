#include <DHT.h>

#define DHTPIN 18
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int sensorPin = 26;  
int sensorValue = 0;                             
float hum = 0.0;
int pcjHum = 0;
int valorSeco   = 3000;   
int valorHumedo = 1100;   

// VALORES DE TEMPERATURA ADECUADA PARA PLANTAS GENERAL
float tempMinPlantas = 5.0;  // Temperatura mínima general
float tempMaxPlantas = 40.0;  // Temperatura máxima general

unsigned long lastReadTime = 0;
const long interval = 5000; 

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Iniciando lectura de sensores...");
  dht.begin();
  pinMode(sensorPin, INPUT);
}

void loop() {
  if (millis() - lastReadTime >= interval) {
    lastReadTime = millis();

    float h = NAN;
    float t = NAN;
    for (int i = 0; i < 5 && (isnan(h) || isnan(t)); i++) {
      t = dht.readTemperature();
      h = dht.readHumidity();
      delay(50);
    }

    if (isnan(h) || isnan(t)) {
      Serial.println("Error obteniendo datos del DHT11 después de varios intentos");
    } else {
      Serial.print("Humedad DHT11: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperatura: ");
      Serial.print(t);
      Serial.println(" °C");

      // ALERTA DE TEMPERATURA ADECUADA PARA PLANTAS GENERAL
      if (t < tempMinPlantas) {
        Serial.println("ALERTA: La temperatura es demasiado baja para la plantas.");
      } else if (t > tempMaxPlantas) {
        Serial.println("ALERTA: La temperatura es demasiado alta para la plantas.");
      } else {
        Serial.println("Temperatura adecuada para las plantas.");
      }
    }

    sensorValue = analogRead(sensorPin);
    pcjHum = map(sensorValue, valorSeco, valorHumedo, 0, 100);
    if (pcjHum < 0) pcjHum = 0;
    if (pcjHum > 100) pcjHum = 100;

    Serial.print("Humedad suelo: ");
    Serial.print(pcjHum);
    Serial.println(" %");

    if (pcjHum < 30) {
      Serial.println("ALERTA: El suelo está seco. ¡Agrega agua!");
    } else if (pcjHum >= 30 && pcjHum <= 60) {
      Serial.println("El suelo tiene humedad adecuada.");
    } else {
      Serial.println("El suelo está muy húmedo.");
    }

    Serial.println("-------------------------------------------------------");
  }
}
