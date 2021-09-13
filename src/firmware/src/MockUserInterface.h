#ifndef __MOCKUSERINTERFACE_H__
#define __MOCKUSERINTERFACE_H__

#include "UserInterface.h"

class MockUserInterface : public UserInterface {

public:
    bool isPowerButtonOn() {
        return true;
    }
};

#endif