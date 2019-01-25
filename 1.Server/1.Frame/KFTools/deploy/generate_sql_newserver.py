#!/usr/bin/python
#-*- coding:utf-8 -*-

import os 
import sys 
import commands
import config
import subprocess
import time

output_file = "./newserver.sql"

dbsvr_root = "../dbsvr"

path_sql = os.path.join(dbsvr_root, "sql")
path_sql_update = os.path.join(dbsvr_root, "sql_update")

def cat_file(origin, file):
	command = "cat "
	command += origin
	command += " >> "
	command += file
	os.system(command)

def append_sql_from_python(file):
	#print file
	#use create_database.sql to create database, avoid warning
	if file == "create_database.py":
		return
	command = "python "
	command += file
	command += " >> "
	command += output_file
	os.system(command)

def gen_sql(path):
	files = os.listdir(path)
	for onefile in files:
		sufix = os.path.splitext(onefile)[1][1:]
		if sufix == "py":
			child = os.path.join(path,onefile)
			if not os.path.isdir(child):
				append_sql_from_python(child)

os.system("echo '' > " + output_file)
cat_file(os.path.join(path_sql, "create_database.sql"), output_file)
gen_sql(path_sql)
gen_sql(path_sql_update)

