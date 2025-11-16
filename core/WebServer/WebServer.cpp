/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "WebServer.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include "bitset"

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
    auto &vm = monitors;   // alias for readability
    const size_t N = vm.size();
    if (N == 0) {
        mg_http_reply(c, 200, "Content-Type: text/html\r\n",
                      "<html><body><h1>No monitors available</h1></body></html>");
        return;
    }

    // ---- Build the HTML page ----
    std::ostringstream html;

    html << R"HTML(
<html>
<head>
    <meta charset="UTF-8">
    <title>Battery Monitor Status</title>
    <meta http-equiv="refresh" content="5"/>

    <style>
        body {
            background: #1e1e1e;
            color: #d4d4d4;
            font-family: Arial, sans-serif;
            padding: 20px;
        }
        h1 {
            color: #4aa3ff;
        }
        table {
            border-collapse: collapse;
            margin-top: 20px;
        }
        th, td {
            padding: 6px 10px;
            border: 1px solid #555;
            text-align: center;
        }
        th {
            background: #333;
            color: #ddd;
        }
        td {
            color: #eee;
        }
        .row-label {
            text-align: left;
            padding-left: 8px;
            font-weight: bold;
            background: #2a2a2a;
        }
        a {
            color: #4aa3ff;
        }
        a:hover {
            text-decoration: underline;
        }
    </style>
</head>

<body>

<h1>Battery Monitor Status</h1>
<p><a href="/log">View Log</a></p>

)HTML";

    // ---- Time section ----

    // current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // run time
    static auto start_time = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = now - start_time;
    double hours = elapsed_seconds.count() / 3600.0;

    html << "<p><b>Current Time:</b> " << std::ctime(&now_time) << "</p>";
    html << "<p><b>Run Time:</b> " << hours << " hours</p>";

    // ---- Main table ----
    html << "<table>";

    // HEADER ROW: Battery Numbers
    html << "<tr><th class='row-label'>Battery Number</th>";
    for (size_t i = 0; i < N; i++) {
        if (i == N - 1) html << "<th>all</th>";
        else html << "<th>" << (i + 1) << "</th>";
    }
    html << "</tr>";

    // Macro-like lambdas to print a table row
    auto row_text = [&](const std::string &label, auto getter) {
        html << "<tr><td class='row-label'>" << label << "</td>";
        for (size_t i = 0; i < N; i++) {
            html << "<td>" << getter(vm[i]) << "</td>";
        }
        html << "</tr>";
    };

    auto row_bits = [&](const std::string &label, auto getter, int bits) {
        html << "<tr><td class='row-label'>" << label << "</td>";
        for (size_t i = 0; i < N; i++) {
            html << "<td>" << std::bitset<16>(getter(vm[i])).to_string().substr(16 - bits) << "</td>";
        }
        html << "<td class='row-label'></td></tr>";
    };

    // ---- ROWS (exactly matching your console order & fields) ----

    row_text("Pack Status",
        [&](auto m){ return monitor::getPackStatusText(m->getPackStatus()); });

    // Failsafe Status (3 bits)
    html << "<tr><td class='row-label'>Failsafe Status</td>";
    for (size_t i = 0; i < N; i++) {
        html << "<td>" << std::bitset<3>(vm[i]->getFailsafeStatus()) << "</td>";
    }
    html << "<td>bits</td></tr>";

    // Contactor Status (6 bits)
    html << "<tr><td class='row-label'>Contactor Status</td>";
    for (size_t i = 0; i < N; i++) {
        html << "<td>" << std::bitset<6>(vm[i]->getVoltTempStatus()) << "</td>";
    }
    html << "<td>bits</td></tr>";

    row_text("Voltage",                [&](auto m){ return m->getVoltage(); });
    row_text("Current",                [&](auto m){ return m->getCurrent(); });
    row_text("Temperature",            [&](auto m){ return m->getTemperature(); });
    row_text("SOC",                    [&](auto m){ return m->getSocPercent(); });
    row_text("SOH",                    [&](auto m){ return m->getSohPercent(); });
    row_text("Energy Remaining",       [&](auto m){ return m->getEnergyRemainingKwh(); });
    row_text("Capacity",               [&](auto m){ return m->getCapacityKwh(); });
    row_text("Max Charge Voltage",     [&](auto m){ return m->getMaxChargeVoltage(); });
    row_text("Min Discharge Voltage",  [&](auto m){ return m->getMinDischargeVoltage(); });
    row_text("Charge Current Limit",   [&](auto m){ return m->getChargeCurrentLimit(); });
    row_text("Discharge Current Lmt",  [&](auto m){ return m->getDischargeCurrentLimit(); });

    html << "</table>";

    html << "</body></html>";

    // ---- SEND PAGE ----
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

    // ---- REASSEMBLE ----
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
        font-family: Arial, sans-serif;  /* SAME FONT AS STATUS PAGE */
        padding: 20px;
    }

    h1, a {
        color: #569cd6;
    }

    pre {
        font-size: 14px;
        line-height: 1.35;
        white-space: pre-wrap;
        padding: 15px;
        background: #1e1e1e;
        border: 1px solid #333;
        border-radius: 4px;

        /* 🔥 Non-monospace log font */
        font-family: Arial, sans-serif;
    }

    a:hover {
        text-decoration: underline;
    }

    /* VSCode-like colors */
    .hljs-keyword,
    .hljs-selector-tag,
    .hljs-literal { color:#569cd6; }

    .hljs-number { color:#b5cea8; }
    .hljs-string { color:#ce9178; }
    .hljs-title, .hljs-name { color:#dcdcaa; }
    .hljs-attribute { color:#9cdcfe; }
    .hljs-comment { color:#6a9955; }
    .hljs-meta { color:#d16d9e; }
    .hljs-type { color:#4ec9b0; }
</style>

<script>
const hljs = {
    highlightAll: function() {
        document.querySelectorAll('pre code').forEach((block) => {
            hljs.highlightBlock(block);
        });
    },
    highlightBlock: function(block) {
        let html = block.innerHTML;

        html = html.replace(/(\[INFO\])/g,  '<span style="color:#4aa3ff;font-weight:bold">$1</span>');
        html = html.replace(/(\[WARN\])/g,  '<span style="color:#ff9800;font-weight:bold">$1</span>');
        html = html.replace(/(\[ERROR\])/g, '<span style="color:#f44747;font-weight:bold">$1</span>');
        html = html.replace(/(\[ALARM\])/g, '<span style="color:#ffea00;font-weight:bold">$1</span>');
        html = html.replace(/(\[DEBUG\])/g, '<span style="color:#4ec9b0">$1</span>');

        html = html.replace(/([A-Za-z0-9_]+\.(cpp|hpp):\d+)/g,
                            '<span style="color:#9cdcfe">$1</span>');

        html = html.replace(/(0x[0-9A-Fa-f]+)/g,
                            '<span style="color:#b5cea8">$1</span>');

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
