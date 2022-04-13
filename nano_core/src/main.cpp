#include <SPIFFS.h>
#include <WiFi.h>
#include <DNSServer.h>

#include "configure.hpp"
#include "motor.hpp"
#include "speed.hpp"
#include "web_server.hpp"

#include "functional"

#define DIR 18
#define STEP 19
#define S1 34
#define S2 35
#define S3 32
#define S4 33

SpeedManager manager(S1, S2, S3, S4);
Motor motor(DIR, STEP);
NanoServer server(80);

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;

void manager_task(void *parameters) {
    while (true) {
        manager.tick();
        server.handle_client();
        dnsServer.processNextRequest();
    }
}

void setup() {
    setCpuFrequencyMhz(240);
    // Serial.begin(115200);

    if (!SPIFFS.begin(true)) {
        // Serial.println("SPIFFS Mount Failed");
        return;
    }

    if (!Config::inst().load(SPIFFS)) {
        // Serial.println("load failed");
        return;
    }

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(Config::inst().data.ssid, Config::inst().data.password, 13, 0,
                1);

    manager.register_delay_change([](uint32_t delay) {
        motor.on_delay_change(delay);
    });

    manager.register_dir_change([](Direction dir) {
        motor.on_dir_change(dir);
    });

    if (!server.init(SPIFFS, &manager)) {
        // Serial.println("server init failed");
        return;
    }

    dnsServer.setTTL(300);
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

    xTaskCreatePinnedToCore(manager_task, "manager_task", 4096, nullptr, 1,
                            nullptr, 0);

    server.begin();
    dnsServer.start(DNS_PORT, "www.nanolight.cn", apIP);
}

void loop() { motor.tick(); }
