#include "configure.hpp"

#include <ArduinoJson.h>

bool DynamicConfig::load_json(const char* str, int len) {
    StaticJsonDocument<CONFIG_STR_LEN> doc;
    auto err = deserializeJson(doc, str);
    if (err) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.c_str());
        return false;
    }
    gearbox_ratio = doc["ratio"];
    strcpy(ssid, doc["ssid"]);
    strcpy(password, doc["pwd"]);
    return true;
}

bool DynamicConfig::dump_json(char* str, int cap_len, int* len) const {
    if (cap_len < CONFIG_STR_LEN) {
        return false;
    }
    sprintf(str, "{\"ssid\": \"%s\", \"pwd\": \"%s\", \"ratio\": %lf}", ssid,
            password, gearbox_ratio);
    *len = strlen(str);
    return true;
}

bool DynamicConfig::load(fs::FS& fs) {
    File config_file = fs.open(CONFIG_PATH);
    if (!config_file.available()) {
        config_file.close();
        // use default config and write back
        use_default_value();
        return save(fs);
    }
    char buffer[CONFIG_STR_LEN];
    int read_size = 0;
    while (
        config_file.readBytes(buffer + read_size, CONFIG_STR_LEN - read_size))
        ;
    config_file.close();
    if (!load_json(buffer, CONFIG_STR_LEN)) {
        use_default_value();
        return save(fs);
    }
}

bool DynamicConfig::save(fs::FS& fs) const {
    File config_file = fs.open(CONFIG_PATH, "w");
    if (!config_file.available()) {
        return false;
    }
    char buffer[CONFIG_STR_LEN];
    int write_size = 0;
    if (!dump_json(buffer, CONFIG_STR_LEN, &write_size)) {
        return false;
    }
    int done = 0;
    while (write_size > done) {
        done += config_file.write((uint8_t*)(buffer + done), write_size);
    }
    config_file.close();
    
    return true;
}

void DynamicConfig::use_default_value() {
    gearbox_ratio = DEFAULT_GEARBOX_RATIO;
    strcpy(ssid, DEFAULT_SSID);
    strcpy(password, DEFAULT_PWD);
}
