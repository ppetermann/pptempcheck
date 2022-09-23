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

bool ensureWifi();

bool ensureMQTT();

// setup
void setup() {
    Wire.begin(SDA_PIN, SCL_PIN);
    u8g2.begin();
    dht.begin();

    // wifi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    ensureWifi();

    // mqtt
    mqttClient.begin(MQTT_BROKER_HOST, wifiClient);
    ensureMQTT();

    delay(500);
}

bool ensureMQTT() {
    // all good if connected
    if (mqttClient.connected()) {
        return true;
    }

    // connect
    mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS);

    // @todo secondary exit, so we can fall back to reconnecting wifi if necessary
    int i = 0;
    while (!mqttClient.connected() && i < 20) {
        blinkConnectionStatus(CONN_SERVICE_MQTT, CONN_STATE_CONNECTING);
        i++;
    }

    if (i < 20 ) {
        blinkConnectionStatus(CONN_SERVICE_MQTT, CONN_STATE_CONNECTED);
        delay(500);
        publishDiscovery();
        return true;
    }
    blinkConnectionStatus(CONN_SERVICE_MQTT, CONN_STATE_FAILED);
    delay(500);
    return false;
}

bool ensureWifi() {
    // all good if connected
    if (WiFi.isConnected()) {
        return true;
    }
    // reconnect
    WiFi.reconnect();
    while (WiFiClass::status() != WL_CONNECTED) {
        blinkConnectionStatus(CONN_SERVICE_WIFI, CONN_STATE_CONNECTING);
    }
    blinkConnectionStatus(CONN_SERVICE_WIFI, CONN_STATE_CONNECTED);
    // @todo add fail
    return true;
}

void loop() {
    // we check Wi-Fi, if Wi-Fi gone we wait for it to come back?
    if(ensureWifi() && ensureMQTT()) {
        u8g2.clearDisplay();
        floatTempInC = readTemp();
        //renderTemp(floatTempInC);
        mqttClient.loop();
        publishTemp(floatTempInC);
        renderTemp(floatTempInC);
    }
    delay(10000);
}

void blinkConnectionStatus(const char *service, const char *status) {
    u8g2.clearBuffer();                    // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);    // choose a suitable font
    u8g2.drawStr(0, 10, service);
    u8g2.drawStr(0, 20, status);    // write something to the internal memory
    u8g2.sendBuffer();                    // transfer internal memory to the display
    delay(500);
    u8g2.clearDisplay();
    u8g2.drawStr(0, 10, service);
    u8g2.sendBuffer();
    delay(500);
    u8g2.clearDisplay();
}


void renderTemp(float temp) {
    char strTempInC[7];
    dtostrf(temp, 5, 2, strTempInC);
    u8g2.clearBuffer();                    // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);    // choose a suitable font
    u8g2.drawStr(0, 10, "Temperature");    // write something to the internal memory
    u8g2.drawStr(0, 20, strTempInC);    // write something to the internal memory
    u8g2.sendBuffer();                    // transfer internal memory to the display
}

float readTemp() {
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
    doc["uniq_id"] = MQTT_CLIENT_ID;
    doc["dev_cla"] = "temperature";
    doc["stat_t"] = MQTT_TOPIC;
    doc["unit_of_meas"] = "°C";
    doc["val_tpl"] = "{{ value_json.temperature }}";
    serializeJson(doc, localDoc);

    mqttClient.publish(MQTT_DISCOVER, localDoc);
}

void publishTemp(float temp) {
    char localDoc[200];
    doc.clear();

    doc["temperature"] = (int)(temp * 100 + 0.5) / 100.0; // NOLINT(bugprone-incorrect-roundings)
    serializeJson(doc, localDoc);

    mqttClient.publish(MQTT_TOPIC, localDoc);
}
