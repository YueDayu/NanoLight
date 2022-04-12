#include "motor.hpp"

#include <Arduino.h>
#include "speed.hpp"

Motor::Motor(int dir_pin, int speed_pin)
    : dir_pin_(dir_pin),
      speed_pin_(speed_pin),
      delay_(0),
      dir_(Direction::STOP) {
    pinMode(dir_pin_, OUTPUT);
    pinMode(speed_pin_, OUTPUT);
}

void Motor::tick() {
    if (dir_ == Direction::STOP) {
        delay(10);
        return;
    }
    digitalWrite(speed_pin_, LOW);
    delayMicroseconds(delay_);
    digitalWrite(speed_pin_, HIGH);
    delayMicroseconds(delay_);
}

void Motor::on_delay_change(uint32_t new_delay) { delay_ = new_delay; }

void Motor::on_dir_change(Direction new_dir) {
    dir_ = new_dir;
    digitalWrite(dir_pin_, dir_ == Direction::POS);
}
