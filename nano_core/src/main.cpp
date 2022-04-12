#include <SPIFFS.h>
#include <WiFi.h>
#include <WebServer.h>

#include "configure.hpp"
#include "motor.hpp"
#include "speed.hpp"

#include "functional"

// WebServer server(80);

#define DIR 18
#define STEP 19
#define S1 34
#define S2 35
#define S3 32
#define S4 33

SpeedManager manager(S1, S2, S3, S4);
Motor motor(DIR, STEP);

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

// void motor_task(void *parameters) {
//     while (true) {
//         motor.tick();
//         vTaskDelay(1);
//     }
// }

void manager_task(void *parameters) {
    while (true) {
        manager.tick();
        delay(1000);
    }
}

void setup() {
    Serial.begin(115200);

    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    listDir(SPIFFS, "/", 0);

    if (!Config::inst().load(SPIFFS)) {
        Serial.println("load failed");
    }

    char buffer[128];
    sprintf(buffer, "%s, %s, %lf", Config::inst().data.ssid,
            Config::inst().data.password, Config::inst().data.gearbox_ratio);
    Serial.println(buffer);

    manager.register_delay_change([](uint32_t delay) {
        Serial.println("delay");
        Serial.println(delay);
        motor.on_delay_change(delay);
    });

    manager.register_dir_change([](Direction dir) {
        Serial.println("dir");
        Serial.println(dir);
        motor.on_dir_change(dir);
    });

    xTaskCreatePinnedToCore(manager_task, "manager_task", 4096, nullptr, 1, nullptr,
                            0);

    // WiFi.softAP("nanolight", "12345678", 13, 0, 1);

    // server.serveStatic("/", SPIFFS, "/index.html");
    // server.serveStatic("/main.58843f1a.js", SPIFFS, "/main.58843f1a.js");
    // server.serveStatic("/main.f7a8434c.css", SPIFFS, "/main.f7a8434c.css");
    // server.serveStatic("/asset-manifest.json", SPIFFS,
    // "/asset-manifest.json");

    // server.begin();

    // xTaskCreatePinnedToCore(web_task, "web_task", 10000, nullptr, 1, nullptr,
    // 0);
}

void loop() {
    motor.tick();
}
