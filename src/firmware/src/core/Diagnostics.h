#ifndef __DIAGNOSTICS_H__
#define __DIAGNOSTICS_H__

#include <Esp.h>
#include <WiFi.h>
#include <SD.h>
#include <SPIFFS.h>

class DiagnosticsClass
{
private:
    u_long startTime;

public:
    uint32_t getHeapSize() { return ESP.getHeapSize();}
    uint32_t getFreeHeapSize() { return esp_get_free_heap_size();}
    uint32_t getLowestFreeHeapSize() { return esp_get_minimum_free_heap_size();}
    uint32_t getLargestFreeHeapBlockSize() { return heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);}

    const char* getChipModel() { return ESP.getChipModel();}
    uint8_t getChipRevision() { return ESP.getChipRevision();}
    uint32_t getCpuFreqMHz() { return ESP.getCpuFreqMHz();}

    String getWiFiSSID() { return WiFi.SSID(); }
    String getWiFiIP() { 
        char buff[16];
        auto ip = WiFi.localIP();

        sprintf(buff, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
        
        return String(reinterpret_cast<char*>(buff));
    }

    int32_t getWiFiRSSI() { return WiFi.RSSI(); }

    int64_t getSDCardSize() { return SD.cardType() ? SD.cardSize() : -1; }
    int64_t getSDTotalBytes() { return SD.cardType() ? SD.totalBytes() : -1; }
    int64_t getSDUsedBytes() { return SD.cardType() ? SD.usedBytes() : -1; }

    uint64_t getSPIFFSTotalBytes() { return SPIFFS.totalBytes(); }
    uint64_t getSPIFFSUsedBytes() { return SPIFFS.usedBytes(); }

    void setStartTime(unsigned long startTime);
    unsigned long getStartTime();
};

extern DiagnosticsClass Diag;

#endif // __DIAGNOSTICS_H__