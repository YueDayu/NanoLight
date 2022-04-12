#ifndef _CONFIGURE_HPP
#define _CONFIGURE_HPP

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
static const uint32_t MOTOR_MIN_DELAY_US = 100;

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
struct DynamicConfig {
    double gearbox_ratio;
    char ssid[11];
    char password[11];

    bool load_json(const char* str, int len);
    bool dump_json(char* str, int cap_len, int* len) const;

    DynamicConfig& operator=(const DynamicConfig& other);
};

class Config {
 public:
    static Config& inst() {
        static Config config;
        return config;
    }

    DynamicConfig data{};

    bool load(fs::FS& fs);
    bool save(fs::FS& fs) const;

 private:
    void use_default_value();
    Config() = default;
};

#endif  // _CONFIGURE_HPP
