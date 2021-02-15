#include "FileUploader.h"


String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void FileUploader::send(File* file, const char* url) 
{
    HTTPClient client;
    File f = *file;

    client.begin(url);
    client.setUserAgent("IoTalkie/Arduino");
    client.addHeader("clientId", "abcdefg");

    int result = client.sendRequest("POST", &f, f.size());

    client.end();
    Serial.print("Upload completed with code: ");
    Serial.println(result);
}
