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
    r = requests.post(url, data=m, headers={'Content-Type': m.content_type})
    
    retjson = json.loads(r.text)
    if retjson['code'] != 0 or retjson['msg'] != 'ok':
        raise Exception('[ERROR] upload [%s] to web server failed, code=%d msg=%s' % (path, retjson['code'], retjson['msg']))
    else:
        print 'upload [%s] to web server success, info = %s' % (path, r.text)

#do_post('http://version.leiwu.com/upload.php', 'sgame_svn_1234567_201808281738.tar.gz')