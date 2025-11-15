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
    WebServer(std::vector<monitor::Monitor*> &monitors);
    ~WebServer();

private:
    static void eventHandler(struct mg_connection *c, int ev, void *ev_data);
    void handleRequest(struct mg_connection *c);

    mg_mgr mgr;
    bool running;
    std::thread serverThread;

    std::vector<monitor::Monitor*> &monitors;
};

}

#endif /* CORE_WEBSERVER_WEBSERVER_HPP_ */
