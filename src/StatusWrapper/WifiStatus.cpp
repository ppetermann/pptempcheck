//
// Created by ppetermann on 24.09.22.
//

#include "StatusWrapper/WifiStatus.h"

bool WifiStatus::isConnected() {
    return WiFi.isConnected();
}

bool WifiStatus::wait() {
    return WiFiClass::status() != WL_CONNECTED;
}

void WifiStatus::connect() {
    WiFi.reconnect();
}

char *WifiStatus::getServiceName() {
    return (char *) CONN_SERVICE_WIFI;
}

void WifiStatus::success() {

}

WifiStatus::WifiStatus() = default;
