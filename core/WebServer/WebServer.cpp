/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "WebServer.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>

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
        //  << "<meta http-equiv='refresh' content='5'/>"
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
    // ---- READ LOG FILE ----
    std::ifstream logFile("BatteryController.log");
    std::string content;

    if (logFile.is_open()) {
        std::stringstream buffer;
        buffer << logFile.rdbuf();
        content = buffer.str();
    } else {
        content = "Unable to open BatteryController.log";
    }

    // ---- SPLIT INTO LINES ----
    std::vector<std::string> lines;
    {
        std::stringstream ss(content);
        std::string line;
        while (std::getline(ss, line)) {
            lines.push_back(line);
        }
    }

    // ---- REVERSE ORDER (NEWEST FIRST) ----
    std::reverse(lines.begin(), lines.end());

    // ---- REASSEMBLE INTO ONE STRING ----
    std::string reversed;
    reversed.reserve(content.size());
    for (auto &line : lines) {
        reversed += line + "\n";
    }

    // ---- ESCAPE HTML ----
    auto escapeHtml = [](const std::string &input) {
        std::string out;
        out.reserve(input.size());
        for (char ch : input) {
            switch (ch) {
                case '&': out += "&amp;"; break;
                case '<': out += "&lt;"; break;
                case '>': out += "&gt;"; break;
                default: out += ch;
            }
        }
        return out;
    };

    std::string escaped = escapeHtml(reversed);

    // ---- BUILD HTML ----
    std::ostringstream html;

    html << R"HTML(
<html>
<head>
    <meta charset="UTF-8">
    <title>BatteryController.log (Newest First)</title>

    <style>
        body {
            background: #1e1e1e;
            color: #d4d4d4;
            font-family: Arial, sans-serif;   /* UI font matches status page */
            padding: 20px;
        }

        h1, a {
            color: #569cd6;
        }

        pre {
            font-size: 13px;
            white-space: pre-wrap;
            padding: 15px;
            background: #1e1e1e;
            border: 1px solid #333;
            border-radius: 4px;

            /* LOG TEXT = MONOSPACE for alignment */
            font-family: "Cascadia Mono", "Consolas", "Courier New", monospace;
        }

        a:hover {
            text-decoration: underline;
        }

        /* VSCode-like syntax colors */
        .hljs-keyword, .hljs-selector-tag, .hljs-literal { color: #569cd6; }
        .hljs-number { color: #b5cea8; }
        .hljs-string { color: #ce9178; }
        .hljs-title, .hljs-name { color: #dcdcaa; }
        .hljs-attribute { color: #9cdcfe; }
        .hljs-comment { color: #6a9955; }
        .hljs-meta { color: #d16d9e; }
        .hljs-type { color: #4ec9b0; }
    </style>

    <!-- Minimal embedded log highlighter -->
    <script>
    const hljs = {
        highlightAll: function() {
            document.querySelectorAll('pre code').forEach((block) => {
                hljs.highlightBlock(block);
            });
        },

        highlightBlock: function(block) {
            let html = block.innerHTML;

            // Log levels
            html = html.replace(/(\[INFO\])/g,  '<span style="color:#4aa3ff;font-weight:bold">$1</span>');
            html = html.replace(/(\[WARN\])/g,  '<span style="color:#ff9800;font-weight:bold">$1</span>');
            html = html.replace(/(\[ERROR\])/g, '<span style="color:#f44747;font-weight:bold">$1</span>');
            html = html.replace(/(\[ALARM\])/g, '<span style="color:#ffea00;font-weight:bold">$1</span>');
            html = html.replace(/(\[DEBUG\])/g, '<span style="color:#4ec9b0">$1</span>');

            // File references
            html = html.replace(/([A-Za-z0-9_]+\.(cpp|hpp):\d+)/g,
                                '<span style="color:#9cdcfe">$1</span>');

            // Hex values
            html = html.replace(/(0x[0-9A-Fa-f]+)/g,
                                '<span style="color:#b5cea8">$1</span>');

            // Numbers
            html = html.replace(/(\b\d+\b)/g,
                                '<span style="color:#b5cea8">$1</span>');

            block.innerHTML = html;
        }
    };
    </script>
</head>

<body onload="hljs.highlightAll()">

    <h1>BatteryController.log (Newest First)</h1>
    <p><a href="/">⬅ Back to Status</a></p>

    <pre><code>
)HTML";

    html << escaped;

    html << R"HTML(
    </code></pre>

</body>
</html>
)HTML";

    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", html.str().c_str());
}

} // namespace core
