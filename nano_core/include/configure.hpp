#pragma once

#include <SPIFFS.h>

/* ======= Static Configuration =======
 * You SHOULD modify this part ONLY IF using different type of motor or other
 * hardware (or u'r not living in earth).
 * CONFIGURATIONS UNDER THIS SECTION WILL NOT CHANGE AFTER COMPILING.
 */
// Time
static const double SEC_PER_DAY = 86164.09894;
static const double DEGREE_PER_SEC = 360. / SEC_PER_DAY;

// Stepper motor
static const double MOTOR_STEP_DEG = 3.75;
static const double MOTOR_STEP_NUM = 16;

// Config path
static const char CONFIG_PATH[] = "/config.json";

// Default Dynamic Config
static const char DEFAULT_SSID[] = "nano-light";
static const char DEFAULT_PWD[] = "12345678";
static const double DEFAULT_GEARBOX_RATIO = 2916;

static const int CONFIG_STR_LEN = 128;

/* ======= Dynamic Configuration =======
 * These config will load from ${StaticConfig.CONFIG_PATH} after initalization.
 */
class DynamicConfig {
 public:
    static DynamicConfig& instance() {
        static DynamicConfig config;
        return config;
    }

    double gearbox_ratio;
    char ssid[11];
    char password[11];

    bool load(fs::FS& fs);
    bool save(fs::FS& fs) const;

    bool load_json(const char* str, int len);
    bool dump_json(char* str, int cap_len, int* len) const;

 private:
    void use_default_value();
    DynamicConfig() = default;
};
