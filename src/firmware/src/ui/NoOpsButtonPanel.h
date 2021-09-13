#ifndef __NOOPSBUTTONPABEL_H__
#define __NOOPSBUTTONPABEL_H__

#include "ButtonPanel.h"

class NoOpsButtonPanel : public ButtonPanel {

public:
    bool isPowerButtonOn() {
        return true;
    }

    void startup() { }  
    void loop() { }  
};

#endif