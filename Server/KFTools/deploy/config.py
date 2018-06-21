#!/usr/bin/python
#-*- coding:utf-8 -*-

class Config:
	def __init__(self, cfg_filename):
		self.__read_config(cfg_filename)

	def __read_config(self, file_name):
		self.copy_direct = {}
		self.copy_compress = {}
		print file_name
		f = open(file_name, 'r')
		lines = f.readlines()
		for line in lines:
			# 注释 注意不要在行尾注释
			if line.find('#') != -1:
				continue
			
			line = line.replace(' ','')
			line = line.replace("\t","")
			line = line.replace("\n","")
			
			# => 直接拷贝
			if line.find("=>") > 0:
				line_split = line.split("=>")
				if len(line_split) == 2:
					self.copy_direct[line_split[0]] = line_split[1]
				continue
			
			# =*> 压缩拷贝
			if line.find("=*>") > 0:
				line_split = line.split("=*>")
				if len(line_split) == 2:
					self.copy_compress[line_split[0]] = line_split[1]
				continue
			
			if line.find("=") > 0:
				line_split = line.split("=")
				if len(line_split) == 2:
					if line_split[0] == "server_ip":
						self.server_ip = line_split[1]
					if line_split[0] == "server_user":
						self.server_user = line_split[1]
					if line_split[0] == "server_pass":
						self.server_pass = line_split[1]
					if line_split[0] == "from_prefix":
						self.from_prefix = line_split[1]
					if line_split[0] == "to_prefix":
						self.to_prefix = line_split[1]
					if line_split[0] == "remote_deploy_tmp_folder":
						self.remote_deploy_tmp_folder = line_split[1]
			
		f.close()
