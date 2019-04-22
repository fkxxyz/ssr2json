#!/usr/bin/python3

import sys
import os
import socket
import signal
import subprocess
import threadpool
import time
import requests
import json
import threading

max_seconds = 10
max_parallel_test = 16
max_test_time = 4

remain = 0

event = threading.Event()


def get_a_free_port():
    s = socket.socket()
    s.bind(('',0))
    port = s.getsockname()[1]
    s.close()
    return port

def test_a_node(node):
    port = get_a_free_port()
    process = subprocess.Popen(['sslocal', "-l", str(port), "-c", node['file']])
    proxies = {'http': 'socks5://127.0.0.1:' + str(port), 'https': 'socks5://127.0.0.1:' + str(port)}
    time.sleep(1)
    
    min_seconds = max_seconds
    for t in range(max_test_time):
        t_start = time.time()
        ok = False
        try:
            jr = requests.get("https://api-ipv4.ip.sb/geoip", proxies=proxies, timeout=max_seconds).text
            if json.loads(jr)['country_code'] != 'CN':
                ok = True
        except:
            ok = False
        if not ok:
            continue
        interval = time.time() - t_start
        if interval < min_seconds:
            min_seconds = interval
    process.send_signal(signal.SIGINT)

    node_info['result'] = -1 if min_seconds == max_seconds else min_seconds

    remain -= 1
    event.set()

def main():
    node_arr = []
    for line in sys.stdin:
        new_node = dict()
        new_node['file'] = line.strip()
        node_arr.append(new_node)
    
    pool = threadpool.ThreadPool(max_parallel_test)
    reqs = threadpool.makeRequests(test_a_node, node_arr)
    [pool.putRequest(req) for req in reqs]
    
    
    remain = len(node_arr)
    while remain > 0:
        event.eait()


if __name__ == '__main__':
    main()


