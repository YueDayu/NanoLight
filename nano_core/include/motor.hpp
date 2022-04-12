#ifndef _MOTOR_HPP
#define _MOTOR_HPP

#include "speed.hpp"

class Motor {
 public:
    Motor(int dir_pin, int speed_pin);

    void tick();

    void on_delay_change(uint32_t new_delay);
    void on_dir_change(Direction new_dir);

 private:
    int dir_pin_;
    int speed_pin_;

    uint32_t delay_;
    Direction dir_;
};

#endif  // _MOTOR_HPP
