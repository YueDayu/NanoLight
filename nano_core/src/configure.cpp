#include "configure.hpp"

#include <ArduinoJson.h>

bool DynamicConfig::load_json(const char* str, int len) {
    StaticJsonDocument<CONFIG_STR_LEN> doc;
    auto err = deserializeJson(doc, str);
    if (err) {
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
    sprintf(str, R"({"ssid": "%s", "pwd": "%s", "ratio": %lf})", ssid, password,
            gearbox_ratio);
    *len = strlen(str);
    return true;
}

DynamicConfig& DynamicConfig::operator=(const DynamicConfig& other) {
    gearbox_ratio = other.gearbox_ratio;
    strcpy(ssid, other.ssid);
    strcpy(password, other.password);
    return *this;
}

bool Config::load(fs::FS& fs) {
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
    if (!data.load_json(buffer, CONFIG_STR_LEN)) {
        use_default_value();
        return save(fs);
    }
    return true;
}

bool Config::save(fs::FS& fs) const {
    File config_file = fs.open(CONFIG_PATH, "w");
    if (!config_file.available()) {
        return false;
    }
    char buffer[CONFIG_STR_LEN];
    int write_size = 0;
    if (!data.dump_json(buffer, CONFIG_STR_LEN, &write_size)) {
        return false;
    }
    int done = 0;
    while (write_size > done) {
        done += config_file.write((uint8_t*)(buffer + done), write_size);
    }
    config_file.close();

    return true;
}

void Config::use_default_value() {
    data.gearbox_ratio = DEFAULT_GEARBOX_RATIO;
    strcpy(data.ssid, DEFAULT_SSID);
    strcpy(data.password, DEFAULT_PWD);
}
