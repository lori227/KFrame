#!/usr/bin/python
# -*- coding:utf-8 -*-

import os
import sys
import time
import paramiko


def ssh_scp_put(ip, myport, user, pwd, local_file, remote_file):
    trans = paramiko.Transport((ip, myport))
    trans.connect(username=user, password=pwd)
    print 'connect ssh ok'
    sftp = paramiko.SFTPClient.from_transport(trans)
    sftp.put(local_file, remote_file)
    trans.close()


def ssh_scp_get(ip, myport, user, pwd, remote_file, local_file):
    trans = paramiko.Transport((ip, myport))
    trans.connect(username=user, password=pwd)
    print 'connect ssh ok'
    sftp = paramiko.SFTPClient.from_transport(trans)
    sftp.get(local_file, remote_file)
    trans.close()

# Test
# ssh_scp_put('192.168.1.9', 22, 'root', '123456#', '/home/nickyang/survival_server/BinEX/tool/gcm/builder/sga    me_201808171813.tar.gz', '/home/sgversion/game_201808171813.tar.gz')
