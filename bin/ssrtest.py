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

max_seconds = 10
max_parallel_test = 16
max_test_time = 4


def get_a_free_port():
    s = socket.socket()
    s.bind(('',0))
    port = s.getsockname()[1]
    s.close()
    return port

def test_a_node(file):
    # file : a json file path
    # return : respone milliseconds
    port = get_a_free_port()
    process = subprocess.Popen(['sslocal', "-l", str(port), "-c", file])
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
    return min_seconds

def test_node_into_info(node_info):
    result = test_a_node(node_info['file'])
    node_info['result'] = 'x' if result == max_seconds else str(result)

def main():
    node_arr = []
    for line in sys.stdin:
        new_node = dict()
        new_node['file'] = line.strip()
        node_arr.append(new_node)
    
    pool = threadpool.ThreadPool(max_parallel_test)
    reqs = threadpool.makeRequests(test_node_into_info, node_arr)
    [pool.putRequest(req) for req in reqs]
    pool.wait()
    print(node_arr)
    

if __name__ == '__main__':
    main()


