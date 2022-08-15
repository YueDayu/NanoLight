#ifndef _WEBSERVER_HPP
#define _WEBSERVER_HPP

#include <WebServer.h>
#include <SPIFFS.h>
#include <functional>

#include "speed.hpp"

static const char WEB_ROOT[] = "/web";

class NanoServer {
 public:
    NanoServer(int port) : server_(port), manager_(nullptr) {}

    bool init(fs::FS& fs, SpeedManager* manager);

    void begin();
    void handle_client();

 private:
    WebServer server_;
    SpeedManager* manager_;

    std::function<bool()> save_func_;

    bool server_dir(fs::FS &fs);

    void handle_status_query();
    void handle_speed_set();
    void handle_config_query();
    void handle_config_set();
    void handle_spin_certain_degree();
};

#endif  // _WEBSERVER_HPP
