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

max_seconds = 2
max_parallel_test = 4
max_test_time = 2

remain = 0
remain_mutex = threading.Lock()
devNull = open(os.devnull, 'w')

event = threading.Event()

queue_out = queue.Queue()

def conv_delay(v):
    if v == 0:
        return 'x'
    else:
        return str(int(v*1000))

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
        retest = True
        ok = False
        while retest:
            try:
                jr = requests.get("https://api-ipv4.ip.sb/geoip", proxies=proxies, timeout=max_seconds).text
                if json.loads(jr)['country_code'] != 'CN':
                    ok = True
                retest = False
            except requests.exceptions.ConnectionError as e:
                if (str(e).find('Max retries exceeded') != -1):
                    retest = True
                    time.sleep(0.5)
                else:
                    ok = False
                    retest = False
            except Exception as e:
                ok = False
                retest = False
        if ok:
            interval = time.time() - t_start
            if interval < min_seconds:
                min_seconds = interval
        else:
            interval = 0
        
        node["tested"] = t
        node['delay_list'].append(conv_delay(interval))
        event.set()
    process.send_signal(signal.SIGINT)
    
    if min_seconds == max_seconds:
        min_seconds = 0
    node['result'] = conv_delay(min_seconds)
    
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
            sys.stdout.write('\r' + str(index + 1) + '\t' + str(node_arr[index]['result']) + ' ' + ' '.join(node_arr[index]['delay_list']) + '\n')
        
        #print('workRequests = ' + str(len(pool.workRequests)))
        #print('workers = ' + str(pool.workers))


if __name__ == '__main__':
    main()


