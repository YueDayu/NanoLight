#include <SPIFFS.h>
#include <WiFi.h>
#include <WebServer.h>

#include "configure.hpp"

// WebServer server(80);

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) {
                listDir(fs, file.name(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

// void web_task(void* parameters) {
//     while (true) {
//         server.handleClient();
//     }
// }

void setup() {
    Serial.begin(115200);

    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    listDir(SPIFFS, "/", 0);

    if (!DynamicConfig::instance().load(SPIFFS)) {
        Serial.println("load failed");
    }

    // listDir(SPIFFS, "/", 0);

    char buffer[128];
    sprintf(buffer, "%s, %s, %lf", DynamicConfig::instance().ssid, DynamicConfig::instance().password, DynamicConfig::instance().gearbox_ratio);
    Serial.println(buffer);

    DynamicConfig::instance().gearbox_ratio = 100;
    if (!DynamicConfig::instance().save(SPIFFS)) {
        Serial.println("save failed");
    }


    if (!DynamicConfig::instance().load(SPIFFS)) {
        Serial.println("load failed");
    }

    sprintf(buffer, "%s, %s, %lf", DynamicConfig::instance().ssid, DynamicConfig::instance().password, DynamicConfig::instance().gearbox_ratio);
    Serial.println(buffer);

    // WiFi.softAP("nanolight", "12345678", 13, 0, 1);

    // server.serveStatic("/", SPIFFS, "/index.html");
    // server.serveStatic("/main.58843f1a.js", SPIFFS, "/main.58843f1a.js");
    // server.serveStatic("/main.f7a8434c.css", SPIFFS, "/main.f7a8434c.css");
    // server.serveStatic("/asset-manifest.json", SPIFFS, "/asset-manifest.json");

    // server.begin();

    // xTaskCreatePinnedToCore(web_task, "web_task", 10000, nullptr, 1, nullptr, 0);
}

void loop() {
}
