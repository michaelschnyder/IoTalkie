#ifndef __HTTPCLIENTEX_H__
#define __HTTPCLIENTEX_H__
#include <HTTPClient.h>

class HTTPClientEx : public HTTPClient {

public:
    int sendRequest(const char * type, Stream * stream, size_t size, std::function<void(unsigned long int, unsigned long int)>);
};

#endif // __HTTPCLIENTEX_H__