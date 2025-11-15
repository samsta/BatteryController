/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "WebServer.hpp"
#include <sstream>


extern "C" int mg_log_level;

namespace core {

WebServer::WebServer(std::vector<monitor::Monitor*> &mons)
    : monitors(mons), running(true)
{
   mg_log_level = 0;      // disable all mongoose logging

    mg_mgr_init(&mgr);

    // Listen on http://0.0.0.0:8080
    mg_http_listen(&mgr, "http://0.0.0.0:8090", WebServer::eventHandler, this);

    // Background thread
    serverThread = std::thread([this]() {
        while (running) {
            mg_mgr_poll(&mgr, 50);
        }
    });
}

WebServer::~WebServer() {
    running = false;
    if (serverThread.joinable())
        serverThread.join();

    mg_mgr_free(&mgr);
}

void WebServer::eventHandler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        WebServer* self = (WebServer*) c->fn_data;
        self->handleRequest(c);
    }
}

// THIS ONE DISPLAYS ALL MONITORS/PACKS
void WebServer::handleRequest(struct mg_connection *c) {
    std::ostringstream html;

    html << "<html><head>"
         << "<meta charset='UTF-8'>"
         << "<title>Battery Monitor</title>"
         << "<style>"
         << "body { font-family: Arial; padding:20px; }"
         << "table { border-collapse: collapse; margin-bottom:30px; }"
         << "th, td { padding: 8px 12px; border: 1px solid #666; }"
         << "th { background: #ddd; }"
         << "</style>"
         << "</head><body>";

    html << "<h1>Battery Monitor Status</h1>";

    for (size_t i = 0; i < monitors.size(); i++)
    {
        auto *m = monitors[i];

        // html << "<h2>" << m->getBatteryName() << "</h2>";
        html << "<h2>Battery " << i << "</h2>";
        html << "<table>";
        html << "<tr><th>SOC (%)</th><th>Voltage (V)</th><th>Current (A)</th><th>Temp (°C)</th></tr>";
        html << "<tr>";

        html << "<td>" << m->getSocPercent()  << "</td>";
        html << "<td>" << m->getVoltage()     << "</td>";
        html << "<td>" << m->getCurrent()     << "</td>";
        html << "<td>" << m->getTemperature() << "</td>";

        html << "</tr>";
        html << "</table>";
    }

    html << "</body></html>";

    std::string out = html.str();
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", out.c_str());
}

// void WebServer::handleRequest(struct mg_connection *c) {
//     std::ostringstream html;

//     html << "<html><head>"
//          << "<meta charset='UTF-8'>"
//          // << "<meta http-equiv='refresh' content='5'/>"
//          << "<title>Battery Monitor</title>"
//          << "<style>"
//          << "body { font-family: Arial; padding:20px; background:black; color:white; }"
//          << "table { border-collapse: collapse; margin-bottom:30px; width: 50%; }"
//          << "th, td { padding: 8px 12px; border: 1px solid #666; color:white; }"
//          << "th { background: #333; }"
//          << "h1, h2 { color:white; }"
//          << "</style>"
//          << "</head><body>";

//     html << "<h1>Battery Monitor Status</h1>";

//     // Show only the LAST monitor
//     if (!monitors.empty()) {
//         monitor::Monitor* m = monitors[monitors.size() - 1];

//         html << "<h2>" << m->getBatteryName() << "</h2>";
//         html << "<table>";
//         html << "<tr><th>SOC (%)</th><th>Voltage (V)</th><th>Current (A)</th><th>Temp (°C)</th></tr>";
//         html << "<tr>";

//         html << "<td>" << m->getSocPercent()  << "</td>";
//         html << "<td>" << m->getVoltage()     << "</td>";
//         html << "<td>" << m->getCurrent()     << "</td>";
//         html << "<td>" << m->getTemperature() << "</td>";

//         html << "</tr>";
//         html << "</table>";
//     }

//     html << "</body></html>";

//     mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", html.str().c_str());
// }


} // namespace core
