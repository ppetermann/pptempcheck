#include "main.h"
#include "config.h"

// display
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// dht sensor
DHT dht(DHTPIN, DHTTYPE);

// wifi
WiFiClient wifiClient;

// mqtt
MQTTClient mqttClient(256);

// Json Doc
StaticJsonDocument<200> doc;

// global vars + init
float floatTempInC = -666;
float previousTemp = -667;

// setup
void setup () {
    Wire.begin(SDA_PIN, SCL_PIN);
    u8g2.begin();
    dht.begin();
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        renderWifiStatus(false);
        delay(500);
        u8g2.clearDisplay();
        delay(500);
    }
    renderWifiStatus(true);
    mqttClient.begin(MQTT_BROKER_HOST, wifiClient);
    mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS);
    while (!mqttClient.connected()) {
        renderWifiStatus(false);
        delay(500);
        u8g2.clearDisplay();
        delay(500);
    }
    publishDiscovery();
    delay(500);
}

void loop() {
    u8g2.clearDisplay();
    floatTempInC = readTemp();
    //renderTemp(floatTempInC);
    mqttClient.loop();
    publishTemp(floatTempInC);
    delay(10000);
}

void renderWifiStatus(bool connected)
{
    u8g2.clearBuffer();					// clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
    if (connected) {
        u8g2.drawStr(0, 20, "connected!");	// write something to the internal memory
    } else {
        u8g2.drawStr(0,20,"..connecting..");	// write something to the internal memory
    }
    u8g2.sendBuffer();					// transfer internal memory to the display

}



void renderTemp(float temp) {
    char strTempInC[7];
    dtostrf(temp, 5, 2, strTempInC);
    u8g2.clearBuffer();					// clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
    u8g2.drawStr(0,10,"Temperature");	// write something to the internal memory
    u8g2.drawStr(0, 20, strTempInC);	// write something to the internal memory
    u8g2.sendBuffer();					// transfer internal memory to the display
}

float readTemp()
{
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));

        // use previous reading
        t = previousTemp;
    }
    previousTemp = t;
    return t;
}

void publishDiscovery() {
    char localDoc[200];
    doc.clear();
    doc["name"] = MQTT_NAME;
    doc["device_class"] = "temperature";
    doc["state_topic"] = MQTT_TOPIC;
    doc["unit_of_measurement"] = "°C";
    doc["value_template"] = "{{ value_json.temperature }}";
    serializeJson(doc, localDoc);
    mqttClient.publish(MQTT_DISCOVER, localDoc);
}

void publishTemp(float temp)
{
    char localDoc[200];
    doc.clear();
    doc["temperature"] = temp;
    serializeJson(doc, localDoc);
    mqttClient.publish(MQTT_TOPIC, localDoc);
}
