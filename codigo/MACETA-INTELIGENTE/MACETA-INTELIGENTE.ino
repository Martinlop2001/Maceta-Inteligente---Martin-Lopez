


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

unsigned long lastReadTime = 0;
const long interval = 5000; 

// Configuraon de plantas

// Estructura para almacenar configuración de cada planta
struct ConfigPlanta {
  char nombre[30];          // Nombre de la planta
  int humedadSueloMin;      // Humedad mínima del suelo (%)
  int humedadSueloMax;      // Humedad máxima del suelo (%)
  int temperaturaMin;       // Temperatura mínima (°C)
  int temperaturaMax;       // Temperatura máxima (°C)
  int humedadAmbienteMin;   // Humedad ambiental mínima (%)
  int humedadAmbienteMax;   // Humedad ambiental máxima (%)
};

ConfigPlanta plantas[3] = {
  {"Lengua de Suegra", 20, 30, 12, 30, 30, 50},  // Configuración Lengua de Suegra
  {"Malamadre", 40, 60, 10, 27, 40, 60},  // Configuración Malamadre
  {"Potus", 30, 50, 15, 30, 40, 70}   // Configuración Potus
};

int plantaSeleccionada = -1;  // Índice de la planta actual

// Funciones del Menú

void mostrarMenu() {
  Serial.println("\nMACETA INTELIGENTE - MENU PLANTAS\n");
  Serial.println("Selecciona una planta:");
  for (int i = 0; i < 3; i++) {
    Serial.print("  [");
    Serial.print(i + 1);
    Serial.print("] ");
    Serial.println(plantas[i].nombre);
  }
  Serial.println("\nIngresa el número (1, 2 o 3):");
}

void seleccionarPlanta() {
  while (plantaSeleccionada == -1) {
    if (Serial.available() > 0) {
      int opcion = Serial.parseInt();
      
      if (opcion >= 1 && opcion <= 3) {
        plantaSeleccionada = opcion - 1;
        Serial.print("\nHas seleccionado: ");
        Serial.println(plantas[plantaSeleccionada].nombre);
        Serial.println("Iniciando monitoreo...\n");
      } else {
        Serial.println("Opción inválida. Ingresa 1, 2 o 3.");
      }
    }
  }
}

void verificarAlertasPlanta(float t, float h) {
  ConfigPlanta &planta = plantas[plantaSeleccionada];
  
  // Verificar temperatura
  if (planta.temperaturaMin > 0 || planta.temperaturaMax > 0) {
    if (t < planta.temperaturaMin) {
      Serial.println("ALERTA: Temperatura muy baja para " + String(planta.nombre));
    } else if (t > planta.temperaturaMax) {
      Serial.println("ALERTA: Temperatura muy alta para " + String(planta.nombre));
    }
  }
  
  // Verificar humedad ambiental
  if (planta.humedadAmbienteMin > 0 || planta.humedadAmbienteMax > 0) {
    if (h < planta.humedadAmbienteMin) {
      Serial.println("ALERTA: Humedad ambiental muy baja para " + String(planta.nombre));
    } else if (h > planta.humedadAmbienteMax) {
      Serial.println("ALERTA: Humedad ambiental muy alta para " + String(planta.nombre));
    }
  }
}

void verificarHumedadSuelo() {
  ConfigPlanta &planta = plantas[plantaSeleccionada];
  
  // Si hay configuración específica, usarla
  if (planta.humedadSueloMin > 0 || planta.humedadSueloMax > 0) {
    if (pcjHum < planta.humedadSueloMin) {
      Serial.println("ATENCIÓN: El suelo está muy seco para " + String(planta.nombre) + ". ¡Agrega agua!");
    } else if (pcjHum > planta.humedadSueloMax) {
      Serial.println("ATENCIÓN: El suelo está muy húmedo para " + String(planta.nombre) + ".");
    } else {
      Serial.println("El suelo tiene humedad adecuada para " + String(planta.nombre) + " ;)");
    }
  } 
}

// Setup y Loop

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  dht.begin();
  pinMode(sensorPin, INPUT);
  
  // Mostrar menú y esperar selección
  mostrarMenu();
  seleccionarPlanta();
}

void loop() {
  if (millis() - lastReadTime >= interval) {
    lastReadTime = millis();

    float h = 0;
    float t = 0;
    
    t = dht.readTemperature();
    h = dht.readHumidity();

    if (isnan(h) || isnan(t)) {
      Serial.println("Error obteniendo datos del DHT11 después de varios intentos");
    } else {
      Serial.print("Humedad DHT11: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperatura: ");
      Serial.print(t);
      Serial.println(" °C");
      
      // Verificar alertas según configuración de la planta
      verificarAlertasPlanta(t, h);
    }

    sensorValue = analogRead(sensorPin);
    pcjHum = map(sensorValue, valorSeco, valorHumedo, 0, 100);
    if (pcjHum < 0) pcjHum = 0;
    if (pcjHum > 100) pcjHum = 100;

    Serial.print("Humedad suelo: ");
    Serial.print(pcjHum);
    Serial.println(" %");

    // Verificar humedad del suelo según la planta seleccionada
    verificarHumedadSuelo();
    
    Serial.println("-------------------------------------------------------");
  }
}
