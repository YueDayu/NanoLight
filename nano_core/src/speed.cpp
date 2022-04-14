#include "speed.hpp"

#include <ArduinoJson.h>
#include <Arduino.h>

#include "configure.hpp"

bool Speed::from_json(const char* str, int len) {
    StaticJsonDocument<128> doc;
    auto err = deserializeJson(doc, str);
    if (err) {
        Serial.println(err.c_str());
        return false;
    }
    int tmp = doc["d"];
    d = Direction(tmp);
    tmp = doc["t"];
    t = SpeedType(tmp);
    s = doc["s"];
    return true;
}

bool Speed::to_json(char* str, int cap_len, int* len) const {
    if (cap_len < SPEED_JSON_STR_LEN) {
        return false;
    }
    sprintf(str, R"({"d":%d,"t":%d,"s":%lf})", d, t, s);
    *len = int(strlen(str));
    return true;
}

double Speed::to_degree() const {
    if (d == Direction::STOP) {
        return 0;
    }
    if (t == SpeedType::DEGREE) {
        return s / 3600;
    }
    return s * DEGREE_PER_SEC;
}

bool RunningStatus::to_json(char* str,
                            int cap_len,
                            int* len,
                            unsigned long cur_time) const {
    if (cap_len < STATUS_JSON_STR_LEN) {
        return false;
    }
    sprintf(str, R"({"s":)");
    int done_len = int(strlen(str));
    int speed_len = 0;
    if (!speed.to_json(str + done_len, cap_len - done_len, &speed_len)) {
        return false;
    }
    done_len += speed_len;
    auto s =
        start_time_ms == 0 ? 0 : (double(cur_time - start_time_ms) / 1000.);
    sprintf(str + done_len, R"(, "t":%.2lf})", s);
    *len = int(strlen(str));
    return true;
}

const double SpeedManager::speed_tab[] = {0, 0, 10, 30, 0.5, 50, 100, 1};

SpeedManager::SpeedManager(int p1, int p2, int p3, int p4)
    : p1_(p1),
      p2_(p2),
      p3_(p3),
      p4_(p4),
      motor_dir_(Direction::STOP),
      motor_delay_(0),
      web_speed_(Direction::STOP, SpeedType::FACTOR, 0) {
    pinMode(p1_, INPUT);
    pinMode(p2_, INPUT);
    pinMode(p3_, INPUT);
    pinMode(p4_, INPUT);
}

bool SpeedManager::register_dir_change(
    std::function<void(Direction)> callback) {
    dir_callback_ = callback;
    return true;
}

bool SpeedManager::register_delay_change(
    std::function<void(uint32_t)> callback) {
    delay_callback_ = callback;
    return true;
}

Speed SpeedManager::get_speed(int id) {
    if (id == 0) {
        return web_speed_;
    } else if (id == 8) {
        return Speed(Direction::STOP, SpeedType::DEGREE, 0);
    }
    auto i = id & 0x7;
    auto dir = neg(Direction(id >> 3));
    if (i == 1) {
        return max_speed(dir);
    }
    return Speed(dir, SpeedType::FACTOR, speed_tab[i]);
}

Direction neg(Direction d) { return Direction(1 - uint32_t(d)); }

Speed SpeedManager::max_speed(Direction dir) {
    double stepper_step =
        MOTOR_STEP_DEG / (MOTOR_STEP_NUM * Config::inst().data.gearbox_ratio);
    if (stepper_step < 0) {
        stepper_step = -stepper_step;
        dir = neg(dir);
    }
    return Speed(dir, SpeedType::DEGREE,
                 1000000 / double(MOTOR_MIN_DELAY_US) * stepper_step);
}

RunningStatus SpeedManager::get_status() const { return status_; }

bool SpeedManager::get_json_status(char* str, int cap_len, int* len) const {
    return status_.to_json(str, cap_len, len, millis());
}

bool SpeedManager::set_web_speed(const Speed& speed) {
    web_speed_ = speed;
    return true;
}

bool SpeedManager::set_web_json_speed(const char* str, int len) {
    return web_speed_.from_json(str, len);
}

void SpeedManager::need_update() { last_status_ = -1; }

void SpeedManager::tick() {
    int cur_status = (digitalRead(p1_) << 3) | (digitalRead(p2_) << 2) |
                     (digitalRead(p3_) << 1) | digitalRead(p4_);
    if (cur_status == last_status_) {
        return;
    }
    auto new_speed = get_speed(cur_status);
    last_status_ = cur_status;

    if (new_speed.s == 0) {
        new_speed.d = Direction::STOP;
    } else if (new_speed.s < 0) {
        new_speed.s = -new_speed.s;
        new_speed.d = neg(new_speed.d);
    }

    Direction new_dir = new_speed.d;
    uint32_t new_delay = 0;
    if (new_dir != Direction::STOP) {
        double stepper_step =
            MOTOR_STEP_DEG /
            (MOTOR_STEP_NUM * Config::inst().data.gearbox_ratio);
        if (stepper_step < 0) {
            new_dir = neg(new_dir);
            stepper_step = -stepper_step;
        }
        new_delay = uint32_t(stepper_step * 1000000 / new_speed.to_degree());
        if (new_delay < MOTOR_MIN_DELAY_US) {
            new_delay = MOTOR_MIN_DELAY_US;
            new_speed = max_speed(new_speed.d);
        }
    }
    status_.speed = new_speed;

    if (new_dir != motor_dir_ || new_delay != motor_delay_) {
        motor_dir_ = new_dir;
        motor_delay_ = new_delay;
        if (new_dir != Direction::STOP) {
            status_.start_time_ms = millis();
        } else {
            status_.start_time_ms = 0;
        }
        if (nullptr != dir_callback_) {
            dir_callback_(motor_dir_);
        }
        if (nullptr != delay_callback_) {
            delay_callback_(motor_delay_);
        }
    }
}
