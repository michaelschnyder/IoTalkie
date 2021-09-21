#include "HealthReporter.h"

void HealthReporter::setup(AzureIoTMqttClient* client, TimeService* timeService) {
    this->client = client;
    this->timeService = timeService;
}

void HealthReporter::loop() {

  if (SEND_STATS_INTERVAL > 0 && millis() - lastSent >= SEND_STATS_INTERVAL) {
    lastSent = millis();
    sendStats();
  }

  if (PRINT_STATS_INTERVAL > 0 && millis() - lastPrint >= PRINT_STATS_INTERVAL) {
    lastPrint = millis();
    printStats();
  }
}

void HealthReporter::printStats() {
    Serial.printf("\nHeap size: %d\n", ESP.getHeapSize());
    Serial.printf("Free Heap: %d\n", esp_get_free_heap_size());
    Serial.printf("Min Free Heap: %d\n", esp_get_minimum_free_heap_size());
    Serial.printf("Largest Free block: %d\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
}

void HealthReporter::sendStats() {

    if (Diag.getStartTime() <= 0) {
        Diag.setStartTime(timeService->getTimestamp() - millis() / 1000);
    }

    char buffer[512];  
    sprintf_P(buffer, STATS_TEMPLATE, 
        timeService->getTimestamp() - Diag.getStartTime(),

        Diag.getFreeHeapSize(),
        Diag.getLargestFreeHeapBlockSize(),

        Diag.getSPIFFSUsedBytes(),
        Diag.getSPIFFSTotalBytes() - Diag.getSPIFFSUsedBytes(),

        Diag.getSDUsedBytes(),
        Diag.getSDTotalBytes() - Diag.getSDUsedBytes(),

        Diag.getWiFiRSSI()
    );

    client->send(buffer);
    lastSent = millis();
}

void HealthReporter::sayHello() {

    if (Diag.getStartTime() <= 0) {
        Diag.setStartTime(timeService->getTimestamp() - millis() / 1000);
    }

    char buffer[512];
   
    sprintf_P(buffer, HELLO_TEMPLATE, 
        BuildInfo::getVersion(), 

        Diag.getStartTime(),
        timeService->getTimestamp() - Diag.getStartTime(),

        Diag.getChipModel(), 
        Diag.getChipRevision(), 
        Diag.getCpuFreqMHz(),
        
        Diag.getHeapSize(),
        Diag.getFreeHeapSize(),
        Diag.getLargestFreeHeapBlockSize(),

        Diag.getSPIFFSTotalBytes(),
        Diag.getSPIFFSUsedBytes(),
        Diag.getSPIFFSTotalBytes() - Diag.getSPIFFSUsedBytes(),

        Diag.getSDCardSize(),
        Diag.getSDTotalBytes(),
        Diag.getSDUsedBytes(),
        Diag.getSDTotalBytes() - Diag.getSDUsedBytes(),

        Diag.getWiFiSSID(),
        Diag.getWiFiRSSI(),
        Diag.getWiFiIP().c_str()
    );

    client->send(buffer);

    lastSent = millis();
}