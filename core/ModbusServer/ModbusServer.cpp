/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "ModbusServer.hpp"
#include "monitor/Monitor.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cmath>
#include <iostream>
#include <fstream>

namespace core {

ModbusServer::ModbusServer(std::vector<monitor::Monitor*> &mons, unsigned short port)
    : m_monitor(mons)
{
    m_running = true;
    m_thread = std::thread(&ModbusServer::serverThread, this, port);
}

ModbusServer::~ModbusServer() {
    m_running = false;

    // close server socket to unblock accept()
    if (m_server_sock != -1) {
        ::shutdown(m_server_sock, SHUT_RDWR);
        ::close(m_server_sock);
        m_server_sock = -1;
    }

    if (m_thread.joinable()) m_thread.join();
}

static ssize_t recv_all(int sock, void *buf, size_t len) {
    size_t off = 0;
    while (off < len) {
        ssize_t r = ::recv(sock, (char*)buf + off, len - off, 0);
        if (r <= 0) return r;
        off += (size_t)r;
    }
    return (ssize_t)off;
}

void ModbusServer::handleClient(int clientSock) {
    // Keep connection open and serve requests until client closes or error
    while (m_running) {
        uint8_t header[7];
        ssize_t r = recv_all(clientSock, header, sizeof(header));
        if (r <= 0) break;

        uint16_t tid = (header[0] << 8) | header[1];
        uint16_t pid = (header[2] << 8) | header[3];
        uint16_t len = (header[4] << 8) | header[5];
        uint8_t uid = header[6];

        if (len == 0) break;
        // len includes unit id + PDU
        size_t pdu_len = (size_t)len - 1;
        std::vector<uint8_t> pdu(pdu_len);
        if (pdu_len > 0) {
            ssize_t r2 = recv_all(clientSock, pdu.data(), pdu_len);
            if (r2 <= 0) break;
        }

        if (pdu_len < 1) break;
        uint8_t func = pdu[0];

        // Only implement Function Code 3 (Read Holding Registers)
        if (func == 3 && pdu_len >= 5) {
            uint16_t start = (pdu[1] << 8) | pdu[2];
            uint16_t qty = (pdu[3] << 8) | pdu[4];
            if (qty == 0) qty = 1;
            if (qty > 125) qty = 125; // Modbus spec max

            // Heartbeat: increment once per request (wraps at 65535)
            m_heartbeat = static_cast<uint16_t>(m_heartbeat + 1);

            // Prepare register values
            std::vector<uint16_t> regs;
            regs.reserve(qty);

            for (uint16_t i = 0; i < qty; ++i) {
                uint16_t addr = start + i;
                uint16_t val = 0;
                if (!m_monitor.empty()) {
                    // Use the last monitor (aggregate / multi-pack) so behavior
                    // matches the WebServer output which shows all monitors including
                    // the combined pack at the end of the vector.
                    monitor::Monitor* m = m_monitor.back();
                    if (m) {
                        // New mapping: 100..104 are the requested control/status registers
                        if (addr >= 100 && addr <= 104) {
                            uint16_t mapped = addr - 100;
                            switch (mapped) {
                                case 0: // 100 = bms_status (0=off,1=running,2=error)
                                    {
                                        auto status = m->getPackStatus();
                                        using Pack = monitor::Monitor::Pack_Status;
                                        if (status == Pack::NORMAL_OPERATION) val = 1;
                                        else if (status != Pack::START_BUTTON_WAIT) val = 2;
                                        else val = 0;
                                    }
                                    break;
                                case 1: // 101 = heartbeat_counter
                                    val = m_heartbeat;
                                    break;
                                case 2: // 102 = max_charge_current_limit_dc (no scaling)
                                    {
                                        float v = m->getChargeCurrentLimit();
                                        if (std::isnan(v)) val = 0;
                                        else {
                                            if (v < 0) v = 0;
                                            if (v > 65535) v = 65535;
                                            val = static_cast<uint16_t>(std::lrint(v));
                                        }
                                    }
                                    break;
                                case 3: // 103 = max_discharge_current_limit_dc (no scaling)
                                    {
                                        float v = m->getDischargeCurrentLimit();
                                        if (std::isnan(v)) val = 0;
                                        else {
                                            if (v < 0) v = 0;
                                            if (v > 65535) v = 65535;
                                            val = static_cast<uint16_t>(std::lrint(v));
                                        }
                                    }
                                    break;
                                case 4: // 104 = soc (no scaling)
                                    {
                                        float v = m->getSocPercent();
                                        if (std::isnan(v)) val = 0;
                                        else {
                                            if (v < 0) v = 0;
                                            if (v > 65535) v = 65535;
                                            val = static_cast<uint16_t>(std::lrint(v));
                                        }
                                    }
                                    break;
                                default:
                                    val = 0;
                                    break;
                            }
                        }
                        else if (addr >= 105 && addr <= 110) {
                            uint16_t mapped = addr - 105; // 0..5
                            switch (mapped) {
                                case 0: // 105 -> Voltage (V * 10)
                                    val = static_cast<uint16_t>(m->getVoltage() * 10.0);
                                    break;
                                case 1: // 106 -> Current (signed int16, A * 10)
                                    {
                                        float cur = m->getCurrent();
                                        if (!std::isnan(cur)) {
                                            int16_t sreg = static_cast<int16_t>(std::lrint(cur * 10.0));
                                            val = static_cast<uint16_t>(static_cast<uint16_t>(sreg));
                                        } else {
                                            val = 0;
                                        }
                                    }
                                    break;
                                case 2: // 107 -> Temperature (signed int16, C * 10)
                                    {
                                        float temp = m->getTemperature();
                                        if (!std::isnan(temp)) {
                                            int16_t sreg = static_cast<int16_t>(std::lrint(temp * 10.0));
                                            val = static_cast<uint16_t>(static_cast<uint16_t>(sreg));
                                        } else {
                                            val = 0;
                                        }
                                    }
                                    break;
                                case 3: // 108 -> SOC (% * 100)
                                    val = static_cast<uint16_t>(m->getSocPercent() * 100.0);
                                    break;
                                case 4: // 109 -> SOH (% * 100)
                                    val = static_cast<uint16_t>(m->getSohPercent() * 100.0);
                                    break;
                                case 5: // 110 -> Energy Remaining (kWh * 100)
                                    val = static_cast<uint16_t>(m->getEnergyRemainingKwh() * 100.0);
                                    break;
                                default:
                                    val = 0;
                                    break;
                            }
                        }
                        else {
                            val = 0;
                        }
                    }
                }
                regs.push_back(val);
            }


            // Build response MBAP + PDU
            size_t byteCount = regs.size() * 2;

            std::vector<uint8_t> out;
            out.reserve(7 + 1 + 1 + 1 + byteCount);

            // MBAP: tid, pid, length
            out.push_back((uint8_t)(tid >> 8));
            out.push_back((uint8_t)(tid & 0xFF));
            out.push_back((uint8_t)(pid >> 8));
            out.push_back((uint8_t)(pid & 0xFF));
            // MBAP length = number of bytes following (Unit ID + PDU)
            uint16_t mbap_len = static_cast<uint16_t>(1 /*unit*/ + 1 /*func*/ + 1 /*bytecount*/ + byteCount);
            out.push_back((uint8_t)(mbap_len >> 8));
            out.push_back((uint8_t)(mbap_len & 0xFF));

            // Unit id
            out.push_back(uid);

            // PDU: function, byte count, data
            out.push_back(3); // function
            out.push_back((uint8_t)byteCount);
            for (auto rreg : regs) {
                out.push_back((uint8_t)(rreg >> 8));
                out.push_back((uint8_t)(rreg & 0xFF));
            }

            // Send
            ssize_t s = ::send(clientSock, out.data(), out.size(), 0);
            (void)s;
        } else {
            // Unsupported - return exception (function code + 0x80 and exception code 1)
            std::vector<uint8_t> out(9);
            out[0] = header[0]; out[1] = header[1]; // tid
            out[2] = header[2]; out[3] = header[3]; // pid
            out[4] = 0; out[5] = 3; // length = unit + func + exc_code (1+1+1)
            out[6] = header[6]; // uid
            out[7] = (uint8_t)(func | 0x80);
            out[8] = 1; // illegal function
            ::send(clientSock, out.data(), out.size(), 0);
        }
    }

    ::shutdown(clientSock, SHUT_RDWR);
    ::close(clientSock);
}

void ModbusServer::serverThread(unsigned short port) {
    int srv = ::socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) {
        return;
    }
    m_server_sock = srv;

    int opt = 1;
    ::setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (::bind(srv, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        ::close(srv);
        m_server_sock = -1;
        return;
    }

    if (::listen(srv, 4) < 0) {
        ::close(srv);
        m_server_sock = -1;
        return;
    }

    // Accept loop
    while (m_running) {
        struct sockaddr_in cliaddr;
        socklen_t clilen = sizeof(cliaddr);
        int client = ::accept(srv, (struct sockaddr*)&cliaddr, &clilen);
        if (client < 0) {
            if (!m_running) break;
            continue;
        }

        // Handle client in a detached thread so we can accept more
        std::thread([this, client]() {
            this->handleClient(client);
        }).detach();
    }

    if (m_server_sock != -1) {
        ::close(m_server_sock);
        m_server_sock = -1;
    }
}

} // namespace core
