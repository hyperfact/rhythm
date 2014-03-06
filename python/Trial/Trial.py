#!/usr/bin/dev python
# coding: GBK
'''
Created on 2014
@author: huaiyu
'''

from __future__ import with_statement

__version__ = "1.0"

import multiprocessing as mp
import os
import sys
import threading as th
import traceback as tb
import time

import urllib
import urllib2
import urlparse
import httplib
import json

import select

import thdp.threadpool as thp
from __builtin__ import callable

prl = th.Lock()

################################################################################
def testHttp():    
    host = 'www.baidu.com'
    http = httplib.HTTPConnection(host)
    try:
        http.request('get', '/')
        #http.request('get', '/', headers = {"Host": "www.google.com",
        #                                "User-Agent": "Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.1) Gecko/20090624 Firefox/3.5",
        #                                "Accept": "text/plain"})
        print http.getresponse().read()
    except httplib.HTTPException as ex:
        print ex
    finally:
        http.close()
        
def testUrl():
    url = "http://www.baidu.com"
    req = urllib2.Request("")
    resp = urllib2.urlopen(req)
    page = resp.read()
    print page

################################################################################    
def jsgCall(req):
    try :
        rsp = urllib2.urlopen(req)
        #print rsp.read()
        jr = json.load(rsp)
        
        with prl:
            print 'status:%r, msg:%r' % (jr['status']['code'],jr['status']['msg'])
    except urllib2.HTTPError as he:
        with prl:
            print he
    except urllib2.URLError as ue:
        with prl:
            print ue
    except Exception as e:
        with prl:
            print e
    pass

def jsgBuildRequest(host,uri,query,t):
    url = 'http://' \
        + host + uri + '?' \
        + 't=' + t \
        + '&' + query \
        + '&__=1390537342'
    #print url
    return urllib2.Request(url)

def jsgThrdRoute(req):
    jsgCall(req)
    pass

def jsgProcRoute(req):
    jsgCall(req)
    pass

def jsgTrainRoute(host,t):
    uri1 = '/api/v2/train/start'
    uri2 = '/api/v2/train/deal'
    q = 'heroid=776965&type=1'
    # heroid:
    # 董卓-500178 荀彧-513818
    # 孙策-600234 刘备-294533
    # 孙权-577146 财神-776965
    # 曹操-865426 周瑜-965554
    jsgCall(host,uri1,q,t)
    jsgCall(host,uri2,q,t)
    pass

def jsgRunThrd(count,token,host,uri=None,query=None,callback=None):
    ts = []
    for _ in range(0,count):
        t = None
        if callback is None:
            req = jsgBuildRequest(host,uri,query,token)
            t = th.Thread(target=jsgThrdRoute,args=(req,))
        else:
            t = th.Thread(target=callback,args=(host,token))
        ts.append(t)
    for t in ts:
        t.start()
    for t in ts:
        t.join()
    pass

def jsgRunProc(count,token,host,uri=None,query=None,callback=None):
    pass

ghost = '122.225.56.187:6175'
#ghost = '115.231.81.70:6165'
#ghost = '115.231.81.70:6285'
gtoken = '0jIHgU0799vrAm0Sk2XLbHZizxkNlqRO4FoCmtVESrmlV66MZx7n47ISwZZAyWwG'
gtask = 'dailytask'
gtcount = 300
gjsgi = {
    'gettoken':     ('/api/v2/zone/select.json', ''),
    'dailytask':    ('/api/v2/activity/everydaylogin/recv.json', 'day=53'),
    'findhero':     ('/api/v2/shop/club/call.json', 'type=&iid=2'), # iid:2-三顾，3-十顾
    'shoot':        ('/api/v2/activity/ymsb/recv', 'type=left'), # type:left,middle,right
    'train':        (jsgTrainRoute,)
}

def jsgRun():
    global gjsgi
    global ghost 
    global gtoken
    global gtask 
    global gtcount

    method = gjsgi[gtask][0]
    query = None
    if callable(method):
        jsgRunThrd(gtcount, gtoken, ghost, callback=method)
    else:
        query = gjsgi[gtask][1]
        jsgRunThrd(gtcount, gtoken, ghost, method, query)
    print 'End task!'
    pass

################################################################################    
def testThrdRoute(param1):
    print "thread[%x]" % (th.current_thread().ident)
    
def testThread():
    ts = []
    for _ in range(0,10):
        t = th.Thread(target=testThrdRoute,args=(1,))
        ts.append(t)
        t.start()
    for t in ts:
        t.join()
    pass

################################################################################
count = {}
def testThreadPoolRoute(arg):
    global prl
    global count
    tid = th.current_thread().ident
    with prl:
        if count.get(tid) is None:
            count[tid] = 1
        else:
            count[tid] += 1
        print 'worker[%x],%r' % (th.current_thread().ident, arg)
    pass

def testThreadPool():
    ts = []
    tp = thp.ThreadPool(4)
    rs = thp.makeRequests(testThreadPoolRoute, range(0,100))
    for r in rs:
        tp.putRequest(r)
    tp.wait();
    
    #global count
    print(count) 
    pass

################################################################################
def mpInit(init):
    pass
    #print "new process: [procid:", os.getpid(), "]"

def mpMapRoute(x):
    return x

def mpApplyRoute(x):
    title = '[procid:%x][threadid:%x] %s' % (os.getpid(), th.current_thread().ident, x)
    cmd = 'start "%s"' % (title)
    os.system(cmd)

    time.sleep(60)
    print "[procid:",os.getpid(),"][threadid:", th.current_thread().ident, "] ", x
    return x

################################################################################
def testMulProcess():
    pool = mp.Pool(processes=4, initializer=mpInit,
                    initargs=(1,), maxtasksperchild=1)
    
    result = []
    for i in xrange(0,10):
        result.append(pool.apply_async(func=mpApplyRoute, args=(i,)))
    #result = pool.map(mpMapRoute, xrange(0,100,1))
    pool.close()
    pool.join()
    
    #for res in result:
    #    print res.get()
    #print result

if __name__ == '__main__':
    try:
        jsgRun()
        #testThreadPool()
        #testMulProcess()
        #testHttp()
        #testUrl()
        pass
    except:
        print tb.print_exc()
