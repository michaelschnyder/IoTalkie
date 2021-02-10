#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "UserInterface.h"

class Application {
    UserInterface ui;

    public:
        Application(UserInterface);
        void run();

};


#endif // __APPLICATION_H__