//
// Created by ppetermann on 24.09.22.
//

#ifndef PPTEMPCHECK_STATUSWRAPPER_H
#define PPTEMPCHECK_STATUSWRAPPER_H


class StatusWrapper {
public:
    virtual bool isConnected() = 0;

    virtual void connect() = 0;

    virtual bool wait() = 0;

    virtual char *getServiceName() = 0;

    virtual void success() = 0;
};


#endif //PPTEMPCHECK_STATUSWRAPPER_H
