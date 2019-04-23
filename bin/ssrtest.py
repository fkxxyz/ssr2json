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
import queue

max_seconds = 4
max_parallel_test = 4
max_test_time = 4

remain = 0
remain_mutex = threading.Lock()
devNull = open(os.devnull, 'w')

event = threading.Event()

queue_out = queue.Queue()

def conv_delay(v):
    if v == 0:
        return 'x'


def get_a_free_port():
    s = socket.socket()
    s.bind(('',0))
    port = s.getsockname()[1]
    s.close()
    return port

def test_a_node(node):
    port = get_a_free_port()
    process = subprocess.Popen(['sslocal', "-l", str(port), "-c", node['file']], stdout = devNull, stderr = devNull)
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
        except :
            ok = False
        if ok:
            interval = time.time() - t_start
            if interval < min_seconds:
                min_seconds = interval
        else:
            min_seconds = 0
        
        node["tested"] = t
        node['delay_list'].append(str(min_seconds))
        event.set()
    process.send_signal(signal.SIGINT)
    
    node['result'] = min_seconds
    
    global remain, remain_mutex
    if remain_mutex.acquire():
        remain -= 1
        remain_mutex.release()
    queue_out.put(node['index'])
    event.set()

def main():
    node_arr = []
    index = 0
    for line in sys.stdin:
        new_node = dict()
        new_node['index'] = index
        new_node['file'] = line.strip()
        new_node['tested'] = 0
        new_node['delay_list'] = []
        node_arr.append(new_node)
        index += 1
    
    pool = threadpool.ThreadPool(max_parallel_test)
    reqs = threadpool.makeRequests(test_a_node, node_arr)
    [pool.putRequest(req) for req in reqs]
    
    global remain
    remain = len(node_arr)
    while remain > 0:
        event.clear()
        event.wait()
        
        while queue_out.qsize() > 0:
            index = queue_out.get()
            sys.stdout.write('\rNo.' + str(index) + '\t' + str(node_arr[index]['result']) + ' ' + ' '.join(node_arr[index]['delay_list']) + '\n')


if __name__ == '__main__':
    main()


