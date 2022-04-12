#ifndef _SPEED_HPP
#define _SPEED_HPP

#include <stdint.h>
#include <functional>

enum Direction { POS = 0, NEG = 1, STOP = 2 };

enum SpeedType { FACTOR = 0, DEGREE = 1 };

static const int32_t SPEED_JSON_STR_LEN = 48;

Direction neg(Direction d);

struct Speed {
    Speed() = default;
    Speed(Direction dir, SpeedType type, double speed)
        : d(dir), t(type), s(speed) {}

    Direction d;
    SpeedType t;
    double s;

    bool from_json(const char* str, int len);
    bool to_json(char* str, int cap_len, int* len) const;

    double to_degree() const;
};

static const int32_t STATUS_JSON_STR_LEN = 128;

struct RunningStatus {
    Speed speed;
    unsigned long start_time_ms;

    bool to_json(char* str,
                 int cap_len,
                 int* len,
                 unsigned long cur_time) const;
};

class SpeedManager {
 public:
    SpeedManager(int p1, int p2, int p3, int p4);

    // used for motor control
    bool register_dir_change(std::function<void(Direction)> callback);
    bool register_delay_change(std::function<void(uint32_t)> callback);

    // webserver
    RunningStatus get_status() const;
    bool get_json_status(char* str, int cap_len, int* len) const;

    bool set_web_speed(const Speed& speed);
    bool set_web_json_speed(const char* str, int len);

    void need_update();

    void tick();

 private:
    Speed get_speed(int id);
    Speed max_speed(Direction dir);

    static const double speed_tab[];

    int p1_;
    int p2_;
    int p3_;
    int p4_;
    RunningStatus status_{};

    Direction motor_dir_;
    uint32_t motor_delay_;

    int last_status_ = -1;

    Speed web_speed_;

    std::function<void(Direction)> dir_callback_;
    std::function<void(uint32_t)> delay_callback_;
};

#endif  // _SPEED_HPP
