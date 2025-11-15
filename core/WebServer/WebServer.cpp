/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "WebServer.hpp"
#include <sstream>
#include <fstream>

extern "C" int mg_log_level;

namespace core {

WebServer::WebServer(std::vector<monitor::Monitor*> &mons)
    : running(true),
      monitors(mons)
{
    // Disable all Mongoose logging to console
    mg_log_level = 0;

    mg_mgr_init(&mgr);

    // Listen on port
    mg_http_listen(&mgr, m_webserverport , WebServer::eventHandler, this);

    // Background thread to poll Mongoose
    serverThread = std::thread([this]() {
        while (running) {
            mg_mgr_poll(&mgr, 50);  // 50 ms
        }
    });
}

WebServer::~WebServer() {
    running = false;

    if (serverThread.joinable()) {
        serverThread.join();
    }

    mg_mgr_free(&mgr);
}

// Static Mongoose event handler
void WebServer::eventHandler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev != MG_EV_HTTP_MSG) {
        return;
    }

    // Get pointer to our WebServer instance
    auto *self = static_cast<WebServer*>(c->fn_data);
    if (!self) return;

    auto *hm = static_cast<mg_http_message*>(ev_data);

    // Route based on URI (latest Mongoose: use mg_strcmp + mg_str)
    if (mg_strcmp(hm->uri, mg_str("/log")) == 0) {
        self->handleLogPage(c, hm);
    } else {
        // Default: status page
        self->handleStatusPage(c, hm);
    }
}

// =================== Status Page ("/") ===================

void WebServer::handleStatusPage(struct mg_connection *c, struct mg_http_message * /*hm*/) {
    std::ostringstream html;

    html << "<html><head>"
         << "<meta charset='UTF-8'>"
         << "<meta http-equiv='refresh' content='5'/>"
         << "<title>Battery Monitor</title>"
         << "<style>"
         << "body { font-family: Arial, sans-serif; padding:20px; "
            "background:black; color:white; }"
         << "table { border-collapse: collapse; margin-bottom:30px; width: 60%; }"
         << "th, td { padding: 8px 12px; border: 1px solid #666; color:white; }"
         << "th { background: #333; }"
         << "h1, h2 { color:white; }"
         << "a { color: #4af; text-decoration: none; }"
         << "a:hover { text-decoration: underline; }"
         << "</style>"
         << "</head><body>";

    html << "<h1>Battery Monitor Status</h1>";
    html << "<p><a href=\"/log\">View Log</a></p>";

    for (size_t i = 0; i < monitors.size(); i++)
    {
        auto *m = monitors[i];
        
        html << "<h2>Battery " << i << "</h2>";
        // html << "<h2>" << m->getName() << "</h2>";
        html << "<table>";
        html << "<tr>"
             << "<th>SOC (%)</th>"
             << "<th>Voltage (V)</th>"
             << "<th>Current (A)</th>"
             << "<th>Temp (°C)</th>"
             << "</tr>";

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

// =================== Log Page ("/log") ===================

void WebServer::handleLogPage(struct mg_connection *c, struct mg_http_message * /*hm*/) {
    std::ostringstream html;

    // NOTE: This assumes BatteryController.log is in the process working dir.
    // If your logger writes elsewhere, adjust this path.
    std::ifstream logFile("BatteryController.log");
    std::string content;

    if (logFile.is_open()) {
        std::stringstream buffer;
        buffer << logFile.rdbuf();
        content = buffer.str();
    } else {
        content = "Unable to open BatteryController.log";
    }

    html << "<html><head>"
         << "<meta charset='UTF-8'>"
         << "<meta http-equiv='refresh' content='5'/>"
         << "<title>Battery Controller Log</title>"
         << "<style>"
         << "body { font-family: monospace; padding:20px; "
            "background:black; color:white; }"
         << "pre { white-space: pre-wrap; font-size: 13px; line-height: 1.4; }"
         << "a { color: #4af; text-decoration: none; }"
         << "a:hover { text-decoration: underline; }"
         << "</style>"
         << "</head><body>";

    html << "<h1>BatteryController.log</h1>";
    html << "<p><a href=\"/\">⬅ Back to Status</a></p>";
    html << "<pre>" << content << "</pre>";

    html << "</body></html>";

    std::string out = html.str();
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", out.c_str());
}

} // namespace core
