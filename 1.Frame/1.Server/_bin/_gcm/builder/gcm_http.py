#!/usr/bin/python
# -*- coding: UTF-8 -*-

from requests_toolbelt import MultipartEncoder
import requests
import json

def do_post(url, path):
    print 'Post file %s to %s' % (path, url)
    m = MultipartEncoder(
        fields = {
            'name' : 'filename',
            'filename' : (path, open(path, 'rb'), 'application/octet-stream')
        })
    #print m
    #print m.content_type
    r = requests.post(url, verify=True, data=m, headers={'Content-Type': m.content_type})
    
    retjson = json.loads(r.text)
    if retjson['code'] != 0 or retjson['msg'] != 'ok':
        raise Exception('[ERROR] upload [%s] to web server failed, code=%d msg=%s' % (path, retjson['code'], retjson['msg']))
    else:
        print 'upload [%s] to web server success, info = %s' % (path, r.text)

#do_post('https://version.laywoodgames.com/upload.php', 'sgame_online_svn_12345678_201808311343.tar.gz')
