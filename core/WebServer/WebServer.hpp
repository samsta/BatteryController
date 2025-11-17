/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CORE_WEBSERVER_WEBSERVER_HPP_
#define CORE_WEBSERVER_WEBSERVER_HPP_

// #include "can/FrameSink.hpp"
// #include "core/Linux/EpollHandler.hpp"
// #include "logging/stream.hpp"
// #include <logging/logging.hpp>
// #include <string>


#pragma once
#include "mongoose.h"
#include <vector>
#include "monitor/Monitor.hpp"
#include <thread>

namespace core
{

class WebServer {
public:
    // monitors is a reference to your vbatterymon vector in main()
    WebServer(std::vector<monitor::Monitor*> &monitors);
    ~WebServer();

    WebServer(const WebServer&) = delete;
    WebServer& operator=(const WebServer&) = delete;

    const char* getWebServerPort() const { return m_webserverport; }

private:
    const char* m_webserverport = "http://0.0.0.0:8888";
    // Mongoose event handler (new API: no fn_data param)
    static void eventHandler(struct mg_connection *c, int ev, void *ev_data);

    // Handlers for specific pages
    void handleStatusPage(struct mg_connection *c, struct mg_http_message *hm);
    void handleLogPage(struct mg_connection *c, struct mg_http_message *hm);

    mg_mgr mgr;
    bool running;
    std::thread serverThread;

    std::vector<monitor::Monitor*> &monitors;
};

} // namespace core
#endif /* CORE_WEBSERVER_WEBSERVER_HPP_ */
