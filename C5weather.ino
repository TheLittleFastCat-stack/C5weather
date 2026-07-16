#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <BH1750.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

#include <ElegantOTA.h>

WebServer server(80);

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

BH1750 bh;

#define led 15

#define otaSW 8

//#define SDA_PIN 9
//#define SCL_PIN 10

#define uS_TO_S_FACTOR 1000000  /* Konverzní faktor pro mikrosekundy (1 sekunda = 1 000 000 mikrosekund) */
#define log_delay 1800

const char* ssid = "O2-Internet-110";
const char* password = "BRW8QXcGdP";

unsigned long ota_progress_millis = 0;
unsigned long delayT = 0;

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  digitalWrite(led, LOW);
  delay(500);
  digitalWrite(led, HIGH);
}

bool ledON = false;

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
    if(ledON == 0){digitalWrite(led, LOW);}
    else if(ledON == 1){digitalWrite(led, HIGH);}
    ledON = !ledON;
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
    digitalWrite(led, HIGH);
    delay(2000);
  } else {
    Serial.println("There was an error during OTA update!");
    digitalWrite(led, LOW);
    delay(2000);
  }
  // <Add your own code here>
}

void setup() {
  Serial.begin(115200);

  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("power on");

  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);

  pinMode(otaSW, INPUT_PULLUP);

  if(digitalRead(otaSW) == HIGH){
    delay(500);

    Wire.begin(9, 10);

    // Initialize AHT20
    if (!aht.begin()) {
      Serial.println("Could not find AHT20? Check wiring");
      
    }
    
    // Initialize BMP280 (Address 0x77)
    if (!bmp.begin(0x77)) {
      Serial.println("Could not find a valid BMP280 sensor, check wiring!");
      
    }

    if (!bh.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23)) {
      Serial.println("Could not find a valid BH1750 sensor, check wiring!");
      
    }

    digitalWrite(led, LOW);

    sensors_event_t hum, temp;
    aht.getEvent(&hum, &temp); // populate temp and humidity objects

    float temperature = temp.temperature;
    float humidity = hum.relative_humidity;
    
    Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" C");
    Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");

    float pressure = bmp.readPressure() / 100.0;
    
    Serial.print("Pressure: "); Serial.print(pressure); Serial.println(" hPa");
    Serial.print("Approx Altitude: "); Serial.print(bmp.readAltitude(1013.25)); Serial.println(" m");

    float light = bh.readLightLevel();
    Serial.print("Light: ");
    Serial.print(light);
    Serial.println(" lx");


    HTTPClient http;

    http.begin("http://10.0.0.94:5000/weather");
    http.addHeader("Content-Type", "application/json");

    String json = "{";
    json += "\"temperature\":" + String(temperature, 2) + ",";
    json += "\"humidity\":" + String(humidity, 2) + ",";
    json += "\"pressure\":" + String(pressure, 2) + ",";
    json += "\"light\":" + String(light, 0);
    json += "}";

    int code = http.POST(json);

    Serial.println(code);

    http.end();


    delay(1500);
    esp_sleep_enable_timer_wakeup(log_delay * uS_TO_S_FACTOR);

    Serial.println("deep sleep");

    esp_deep_sleep_start();
  }  
  else{
    Serial.println(WiFi.localIP());

    server.on("/", []() {
      server.send(200, "text/plain", "Hi! This is ElegantOTA Demo.");
    });

    ElegantOTA.begin(&server);    // Start ElegantOTA
    // ElegantOTA callbacks
    ElegantOTA.onStart(onOTAStart);
    ElegantOTA.onProgress(onOTAProgress);
    ElegantOTA.onEnd(onOTAEnd);

    server.begin();
    Serial.println("HTTP server started");
  }
}

void loop() {
  
  server.handleClient();
  ElegantOTA.loop();

  if (millis() - delayT > 1000) {
    delayT = millis();
    if(ledON == 0){digitalWrite(led, LOW);}
    else if(ledON == 1){digitalWrite(led, HIGH);}
    ledON = !ledON;
  }
}