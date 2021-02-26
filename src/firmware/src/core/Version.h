#ifndef __VERSION_H__
#define __VERSION_H__

// #ifndef GIT_VERSION
// #define GIT_VERSION "00000";
// #endif

class Version {

public:
    static const char* gitCommit() {
        return GIT_REV;
    }
};
#endif // __VERSION_H__