#include <ArduinoJson.h>

#include "web_server.hpp"
#include "configure.hpp"
#include "speed.hpp"

bool NanoServer::init(fs::FS& fs, SpeedManager* manager) {
    manager_ = manager;
    save_func_ = [&fs]() {
        return Config::inst().save(fs);
    };
    if (!server_dir(fs)) {
        Serial.println("failed to server static");
        return false;
    }
    server_.on("/status", HTTP_POST, [this]() { this->handle_status_query(); });
    server_.on("/set_status", HTTP_POST,
               [this]() { this->handle_speed_set(); });
    server_.on("/config", HTTP_POST, [this]() { this->handle_config_query(); });
    server_.on("/set_config", HTTP_POST,
               [this]() { this->handle_config_set(); });
    server_.on("/spin", HTTP_POST,
               [this]() { this->handle_spin_certain_degree(); });
    return true;
}

void NanoServer::begin() { server_.begin(); }

void NanoServer::handle_client() { server_.handleClient(); }

bool NanoServer::server_dir(fs::FS& fs) {
    File root = fs.open(WEB_ROOT);
    if (!root) {
        Serial.println("failed to open www");
        return false;
    }
    if (!root.isDirectory()) {
        Serial.println(" - not a directory");
        return false;
    }
    File file = root.openNextFile();
    auto folder_len = strlen(WEB_ROOT);
    server_.serveStatic("/", fs, "/web/index.html");
    while (file) {
        if (!file.isDirectory()) {
            auto filename = file.name();
            server_.serveStatic(filename + folder_len, fs, filename);
        }
        file = root.openNextFile();
    }
    return true;
}

void NanoServer::handle_status_query() {
    char buf[STATUS_JSON_STR_LEN];
    int len;
    if (!manager_->get_json_status(buf, STATUS_JSON_STR_LEN, &len)) {
        server_.send(500, "text/json", R"({"ret": false})");
    }
    server_.send(200, "text/json", buf);
}

void NanoServer::handle_speed_set() {
    String arg = server_.arg(0);
    if (!manager_->set_web_json_speed(arg.c_str(), arg.length())) {
        server_.send(500, "text/json", R"({"ret": false})");
    }
    manager_->need_update();
    handle_status_query();
}

void NanoServer::handle_spin_certain_degree() {
    String arg = server_.arg(0);
    StaticJsonDocument<128> doc;
    auto err = deserializeJson(doc, arg.c_str());
    if (err) {
        Serial.println(err.c_str());
        server_.send(500, "text/json", R"({"ret": false})");
    }
    double deg = doc["deg"];

    // Only allow positive degree to simplify logic
    if (!(deg >= 0 && deg < 360)) {
        server_.send(500, "text/json", R"({"ret": false})");
    }
    int tmp = doc["d"];
    Direction d = (deg == 0) ? Direction::STOP : Direction(tmp);

    double stepper_step =
        MOTOR_STEP_DEG / (MOTOR_STEP_NUM * Config::inst().data.gearbox_ratio);
    double duration_ms = deg / (1000 / double(MOTOR_MIN_DELAY_US) * stepper_step);
    if (!manager_->set_web_max_speed(d)) {
        server_.send(500, "text/json", R"({"ret": false})");
    }
    manager_->start_ticker(duration_ms);
    manager_->need_update();
    handle_config_query();
}

void NanoServer::handle_config_query() {
    char buf[CONFIG_STR_LEN];
    int len;
    if (!Config::inst().data.dump_json(buf, CONFIG_STR_LEN, &len)) {
        server_.send(500, "text/json", R"({"ret": false})");
    }
    server_.send(200, "text/json", buf);
}

void NanoServer::handle_config_set() {
    String arg = server_.arg(0);
    if (!Config::inst().data.load_json(arg.c_str(), arg.length())) {
        server_.send(500, "text/json", R"({"ret": false})");
    }
    if (save_func_) {
        if (!save_func_()) {
            server_.send(500, "text/json", R"({"ret": false})");
        }
    }
    manager_->need_update();
    handle_config_query();
}
