#!/usr/bin/env python3
"""Simple Modbus TCP test client (reads holding registers).

Usage:
  python3 tools/modbus_test.py --host 192.168.2.110 --port 5021 --unit 1 --start 100 --count 6

This script builds a raw MBAP + PDU Read Holding Registers request (function 3)
and prints the returned registers and scaled values matching the server mapping.
"""

import socket
import argparse
import struct
import sys


def recv_all(sock, n):
    buf = b""
    while len(buf) < n:
        chunk = sock.recv(n - len(buf))
        if not chunk:
            return None
        buf += chunk
    return buf


def build_request(tid, unit, start, count):
    # MBAP: tid(2), pid(2)=0, length(2), unit(1)
    # PDU: func(1)=3, start(2), count(2)
    pdu = struct.pack('>BHH', 3, start, count)
    length = 1 + len(pdu)  # unit + pdu
    mbap = struct.pack('>HHHB', tid, 0, length, unit)
    return mbap + pdu


def parse_response(resp):
    # resp contains MBAP + body already
    if len(resp) < 9:
        raise ValueError('response too short')
    tid, pid, length = struct.unpack('>HHH', resp[0:6])
    unit = resp[6]
    func = resp[7]
    if func & 0x80:
        exc = resp[8]
        raise ValueError(f'Modbus exception: code {exc}')
    bytecount = resp[8]
    data = resp[9:9+bytecount]
    regs = []
    for i in range(0, len(data), 2):
        regs.append(struct.unpack('>H', data[i:i+2])[0])
    return tid, pid, unit, func, regs


def scale_register(addr, raw):
    if addr == 100:
        return raw / 10.0, 'V'
    if addr == 101:
        return raw / 10.0, 'A'
    if addr == 102:
        return raw / 10.0, '°C'
    if addr == 103:
        return raw / 100.0, '%'
    if addr == 104:
        return raw / 100.0, '%'
    if addr == 105:
        return raw / 100.0, 'kWh'
    return raw, 'raw'


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--host', required=True)
    p.add_argument('--port', type=int, default=5021)
    p.add_argument('--unit', type=int, default=1)
    p.add_argument('--start', type=int, default=100)
    p.add_argument('--count', type=int, default=6)
    p.add_argument('--timeout', type=float, default=5.0)
    args = p.parse_args()

    tid = 1
    req = build_request(tid, args.unit, args.start, args.count)

    try:
        s = socket.create_connection((args.host, args.port), timeout=args.timeout)
    except Exception as e:
        print('connect error:', e)
        sys.exit(2)

    try:
        s.sendall(req)
        # read MBAP first
        mbap = recv_all(s, 6)
        if mbap is None:
            print('no mbap response')
            return
        tid_r, pid_r, length = struct.unpack('>HHH', mbap)
        body = recv_all(s, length)
        if body is None:
            print('incomplete body')
            return
        resp = mbap + body
        tid, pid, unit, func, regs = parse_response(resp)

        print(f'Response TID={tid} PID={pid} UNIT={unit} FUNC={func}')
        for i, r in enumerate(regs):
            addr = args.start + i
            scaled, unit_label = scale_register(addr, r)
            print(f'R{addr:03d}: raw=0x{r:04x} ({r}) -> {scaled} {unit_label}')

    finally:
        s.close()


if __name__ == '__main__':
    main()
