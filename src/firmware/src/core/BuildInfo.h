#ifndef __VERSION_H__
#define __VERSION_H__

#ifndef BUILDVAR_GIT_SHORTREV
#define BUILDVAR_GIT_SHORTREV "c0a0d26"
#endif

#ifndef BUILDVAR_GMT
#define BUILDVAR_GMT "Now"
#endif

#ifndef BUILDVAR_DATEVERSION
#define BUILDVAR_DATEVERSION "2109.16"
#endif

#ifndef BUILDVAR_HOST
#define BUILDVAR_HOST "Somewhere"
#endif

#ifndef BUILDVAR_USER
#define BUILDVAR_USER "Anonynous"
#endif


class BuildInfo {

public:
    static const char* gitCommit() { return BUILDVAR_GIT_SHORTREV; }
    static const char* buildDateVersion() { return BUILDVAR_DATEVERSION; }
    static const char* buildTimeGmt() { return BUILDVAR_GMT; }
    static const char* buildHost() { return BUILDVAR_HOST; }
    static const char* buildUser() { return BUILDVAR_USER; }
};
#endif // __VERSION_H__