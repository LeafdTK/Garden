#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Replace with the IP address of your server and the port
const char* serverName = "http://YOUR_SERVER_IP:3000/upload";

// DHT Sensor setup
#define DHTPIN 2      // GPIO2 (D4 on NodeMCU)
#define DHTTYPE DHT11 // Define the sensor type

DHT dht(DHTPIN, DHTTYPE);

// Function declarations
void connectToWiFi();
void readDHTSensor(int &temperature, int &humidity);
void sendDataToServer(int temperature, int humidity);

void setup() {
    Serial.begin(115200);
    delay(10);

    connectToWiFi();
    dht.begin();
}

void loop() {
    delay(2000);

    int temperature, humidity;
    readDHTSensor(temperature, humidity);

    if (temperature != -1 && humidity != -1) {
        sendDataToServer(temperature, humidity);
    }
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.print(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");
}

void readDHTSensor(int &temperature, int &humidity) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
        Serial.println("Failed to read from DHT sensor!");
        temperature = -1;
        humidity = -1;
    } else {
        temperature = static_cast<int>(t * 10);
        humidity = static_cast<int>(h * 10);
        Serial.print("Humidity: ");
        Serial.print(humidity / 10.0);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(temperature / 10.0);
        Serial.println(" *C");
    }
}

void sendDataToServer(int temperature, int humidity) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;

        String serverPath = String(serverName) + "?temperature=" + String(temperature) + "&humidity=" + String(humidity);

        http.begin(client, serverPath.c_str());
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
        } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }
}
