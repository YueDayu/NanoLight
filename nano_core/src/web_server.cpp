#include "web_server.hpp"

#include "configure.hpp"

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
