#!/usr/bin/python
#-*- coding:utf-8 -*-

import os 
import sys 
import commands
import config
import subprocess
import time
import getpass

DEBUG = False

def debug_print(msg):
	if DEBUG:
		print msg

############################ 通用函数部分 ############################
# 输入的密码里有一些特殊字符的时候，会乱码
def trans_string(strinput):
	strret = strinput.replace('\\','\\\\')
	strret = strret.replace('!','\!')
	return strret

# 调用本地shell命令
def DoShellCmd(cmd, show_result = True):
	debug_print("DoShellCmd cmd: " + cmd)
	(exitcode, outtext) = commands.getstatusoutput(cmd)
	if exitcode != 0:
		print outtext
		#exit(exitcode)
	elif show_result:
		print outtext
	return outtext

# 利用sshpass调用远程shell命令
def DoRemoteCmd(ip, passwd, user, rcmd):
	cmd = "sshpass -p "+passwd+" ssh -p22 -o StrictHostKeyChecking=no "
	cmd += "-l"+user+" "+ip+" \""+rcmd+"\" < /dev/null"
	debug_print("DoRemoteCmd cmd: " + cmd)
	return DoShellCmd(cmd)

# 利用sshpass工具拷贝文件到远程路径
def ScpFileToRemote(ip, passwd, user, src_file, dest_dir, compress = False):
	scp_cmd = "sshpass -p " + passwd
	if compress:
		scp_cmd += " scp -P22 -C -o StrictHostKeyChecking=no "
	else:
		scp_cmd += " scp -P22 -o StrictHostKeyChecking=no "
	scp_cmd += src_file + " " + user + "@" + ip + ":" + dest_dir + " < /dev/null"
	debug_print("ScpFileToRemote cmd: " + scp_cmd)
	DoShellCmd(scp_cmd, False)

# 判定本地和远程两文件是否一样
def RemoteLocalFileMd5Equal(svr_ip, svr_passwd, svr_user, remote_fullname, local_fullname):
	remote_md5sum = DoRemoteCmd(svr_ip, svr_passwd, svr_user, "md5sum -b " + remote_fullname)
	remote_md5sum = remote_md5sum.split()[0]
	local_md5sum = DoShellCmd("md5sum -b " + local_fullname)
	local_md5sum = local_md5sum.split()[0]
	if local_md5sum == remote_md5sum:
		return True
	return False

def DoCopyFiles(cfg):
	for (fname,tname) in cfg.copy_direct.items():
		#print "k:[" + k + "]v:[" + v
		if fname[0] == '/':
			from_name = fname
		else:
			from_name = os.path.join(cfg.from_prefix, fname)
		if tname[0] == '/':
			to_name = tname
		else:
			to_name = os.path.join(cfg.to_prefix, tname)
		
		# 如果md5相等，则不用拷贝
		md5equal = RemoteLocalFileMd5Equal(cfg.server_ip, cfg.server_pass, cfg.server_user, to_name, from_name)
		if md5equal:
			continue
		
		DoRemoteCmd(cfg.server_ip, cfg.server_pass, cfg.server_user, "mkdir -p " + os.path.dirname(to_name))
		
		ScpFileToRemote(cfg.server_ip, cfg.server_pass, cfg.server_user, from_name, os.path.dirname(to_name))
		
	for (fname,tname) in cfg.copy_compress.items():
		#print "k:[" + k + "]v:[" + v
		if fname[0] == '/':
			from_name = fname
		else:
			from_name = os.path.join(cfg.from_prefix, fname)
		if tname[0] == '/':
			to_name = tname
		else:
			to_name = os.path.join(cfg.to_prefix, tname)
		
		# 如果md5相等，则不用拷贝
		md5equal = RemoteLocalFileMd5Equal(cfg.server_ip, cfg.server_pass, cfg.server_user, to_name, from_name)
		if md5equal:
			continue
		
		DoRemoteCmd(cfg.server_ip, cfg.server_pass, cfg.server_user, "mkdir -p " + os.path.dirname(to_name))
		
		ScpFileToRemote(cfg.server_ip, cfg.server_pass, cfg.server_user, from_name, os.path.dirname(to_name), True)

def Deploy():
	#'检查参数'
	if len(sys.argv) < 3:
		print "Usage: python deploy.py configfilename [DEBUG]"
		print "Options: "
		print "[configfilename] : config.conf"
		print "[DEBUG] : debug|DEBUG|Debug"
		sys.exit(1)

	configfilename = sys.argv[1]
	if len(sys.argv) >= 3:
		input_debug = sys.argv[2]
		if input_debug.lower() == "debug":
			global DEBUG
			DEBUG = True
	cfg = config.Config(configfilename)
	# 如果密码没有设置，输入密码
	if cfg.server_pass == "notset":
		cfg.server_pass = trans_string(getpass.getpass("input password:"))
	
	#不要做逻辑，只负责拷贝文件
	DoRemoteCmd(cfg.server_ip, cfg.server_pass, cfg.server_user, "cd " + cfg.to_prefix + " && sh x.sh ")
	DoCopyFiles(cfg)
	DoRemoteCmd(cfg.server_ip, cfg.server_pass, cfg.server_user, "cd " + cfg.to_prefix + " && sh c.sh ")

############################################
Deploy()



