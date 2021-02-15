#include "FileUploader.h"
typedef struct UploadTaskParam {
    File* file;
    char* url;
};

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
void sendBackground(void *arg)
{
    UploadTaskParam* args = (UploadTaskParam*)arg;
    
    Serial.print("Url: ");
    Serial.println(args->url);

    Serial.print("Filesize: ");
    Serial.println(args->file->size());

    HTTPClient client;
//    File f = *args->file;

    client.begin(args->url);
    client.setUserAgent("IoTalkie/Arduino");
    client.addHeader("clientId", "abcdefg");

    Serial.print("Upload starting");
    int result = client.sendRequest("POST", args->file, args->file->size());

    client.end();
    Serial.print("Upload completed with code: ");
    Serial.println(result);

    //free(args->url);
    free(arg);
    vTaskDelete(NULL);
}

void FileUploader::send(File* file, const char* url) 
{
    UploadTaskParam *struct1 = (UploadTaskParam*)malloc(sizeof(UploadTaskParam));
    struct1->file = file;
    
    struct1->url = (char*)malloc(strlen(url) +1);
    strcpy(struct1->url, url);

    xTaskCreate(sendBackground, "sendBackground", 16 * 1024, struct1, 1, NULL);

}
