#ifndef __FILEINFO_H__
#define __FILEINFO_H__

class FileInfo {

public:
    /* Returns a pointer to the extension of 'string'.
    * If no extension is found, returns a pointer to the end of 'string'. */
    static char* getExtension(const char *string)
    {
        assert(string != NULL);
        char *ext = strrchr(string, '.');
        
        if (ext == NULL)
            return (char*) string + strlen(string);
    
        for (char *iter = ext + 1; *iter != '\0'; iter++) {
            if (!isalnum((unsigned char)*iter))
                return (char*) string + strlen(string);
        }
    
        return ext;
    }
};
#endif // __FILEINFO_H__