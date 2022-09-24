//src
// Created by ppetermann on 24.09.22.
//

#ifndef PPTEMPCHECK_WIFISTATUS_H
#define PPTEMPCHECK_WIFISTATUS_H

#include <WiFi.h>
#include "StatusWrapper.h"
#include "states.h"


class WifiStatus : public StatusWrapper {
public:
    explicit WifiStatus();
    bool isConnected() override;
    void connect() override;
    bool wait() override;
    char* getServiceName() override;
    void success() override;
protected:
    WiFiClient wiFiClient;
};


#endif //PPTEMPCHECK_WIFISTATUS_H
