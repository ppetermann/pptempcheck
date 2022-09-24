//
// Created by ppetermann on 24.09.22.
//

#include "MQTTPublisher.h"

MQTTPublisher::MQTTPublisher(MQTTClient &mqttClient) : mqttClient(mqttClient) {

}

void MQTTPublisher::publishDiscovery() {
    char localDoc[200];
    jsonDoc.clear();

    jsonDoc["name"] = MQTT_NAME;
    jsonDoc["uniq_id"] = MQTT_CLIENT_ID;
    jsonDoc["dev_cla"] = "temperature";
    jsonDoc["stat_t"] = MQTT_TOPIC;
    jsonDoc["unit_of_meas"] = "°C";
    jsonDoc["val_tpl"] = "{{ value_json.temperature }}";
    serializeJson(jsonDoc, localDoc);

    mqttClient.publish(MQTT_DISCOVER, localDoc);
}

void MQTTPublisher::publishTemp(float temp) {
    char localDoc[200];
    jsonDoc.clear();

    jsonDoc["temperature"] = (int) (temp * 100 + 0.5) / 100.0; // NOLINT(bugprone-incorrect-roundings)
    serializeJson(jsonDoc, localDoc);

    mqttClient.publish(MQTT_TOPIC, localDoc);
}