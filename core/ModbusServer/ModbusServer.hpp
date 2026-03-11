/* SPDX-License-Identifier: GPL-3.0-or-later */
//********************************************** */
// LOOK IN THE tools DIRECTORY FOR A PYTHONG SCRIPT 
// TO TEST THE MODBUS SERVER
//********************************************** */

#pragma once

#include <vector>
#include <thread>
#include <atomic>
#include <cstdint>

namespace monitor { class Monitor; }

namespace core {

class ModbusServer {
public:
    // listens on tcp://0.0.0.0:1502 (non-privileged alternative to 502)
    ModbusServer(std::vector<monitor::Monitor*> &mons, unsigned short port = 5021);
    ~ModbusServer();

    ModbusServer(const ModbusServer&) = delete;
    ModbusServer& operator=(const ModbusServer&) = delete;

private:
    void serverThread(unsigned short port);
    void handleClient(int clientSock);

    std::vector<monitor::Monitor*> &m_monitor;
    std::thread m_thread;
    std::atomic<bool> m_running{false};
    int m_server_sock{-1};
    uint16_t m_heartbeat{0};
};

} // namespace core
