#!/usr/bin/python
# -*- coding: UTF-8 -*-

# author: Nick Yang
# date: 2018/8/8
# last update time: 2018/8/21
# version: 0.4
# changelog:
#   1.打包版本
#   2.增加只在linux下打包版本
#   3.增加开启服务器的shell
#   4.startup配置和其他配置分开
#   5.增加内外网配置区分 2018/8/31


import os
import os.path 
import sys
import glob
import datetime
import platform
import shutil
import socket
import commands
import argparse
import gcm_xml
import gcm_conf
import gcm_global_conf
import gcm_db
import gcm_http

input_folder = '../conf_input'
output_folder = '../conf_output'

base_path = os.path.join('../../../')
bin_path = 'bin'
setting_path = 'setting'
config_path = 'config'
startup_path = 'startup'
lib_path = 'lib'
script_path = 'script'
win_other_dll_path = os.path.join('../../../../Server/_lib/win64/3rd')

default_startup = 'KFStartup'
default_mode_suffix = 'd'  # debug release后缀

def is_linux():
    if platform.system() == 'Windows':
        return False
    else:
        return True

platform_exe = ''  # executable suffix
if is_linux():
    platform_exe = ''
else:
    platform_exe = '.exe'

run_shell_file = 'run' # bat/shell
kill_shell_file = 'kill'
shell_ext = '' # shell extension
if is_linux():
    shell_ext = '.sh'
else:
    shell_ext = '.bat'

def write_file(run_file, kill_file, node):
    func_name = node.get('FuncName')
    func_id = node.get('FuncID')

    channel_id = args['channel']
    zone_id = args['zone']
    log_type = args['log']
    service_type = args['service']
    if func_name in g_proc_config.cluster_procs:
        zone_id = 0

    execute_file = default_startup + default_mode_suffix + platform_exe
    run_file.write('echo Starting [%s] server\n' % func_name)
    kill_file.write('echo Killing  [%s] server\n' % func_name)
    if is_linux():
        run_file.write('./bin/%s appid=%d.%d.%d.%d log=%s service=%s startup=./startup/%s.startup\n\n' %
                       (execute_file, channel_id, zone_id, int(func_id), 1,log_type, service_type, func_name))

        kill_file.write('ps -ef|grep "%s appid=%d.%d.%d" | grep -v grep | cut -c 9-15 | xargs kill -9\n\n' %
                        (execute_file, channel_id, zone_id, int(func_id)))
    else:
        run_file.write('start "%s" bin\\%s appid=%d.%d.%d.%d log=%s service=%s startup=./startup/%s.startup\n\n' %
                       (func_name, execute_file, channel_id, zone_id, int(func_id), 1, log_type, service_type,func_name))

        kill_file.write('TASKKILL /F /FI "WINDOWTITLE eq %s.*"\n\n' % func_name)

def copy_version():
    shutil.copytree(base_path + bin_path, os.path.join(output_folder, bin_path))
    shutil.copytree(base_path + setting_path, os.path.join(output_folder, setting_path))
    shutil.copytree(base_path + config_path, os.path.join(output_folder, config_path))
    shutil.copytree(base_path + startup_path, os.path.join(output_folder, startup_path))
    shutil.copytree(base_path + lib_path, os.path.join(output_folder, lib_path))
    shutil.copytree(base_path + script_path, os.path.join(output_folder, script_path))

    if is_linux():
        return
    else:
        shutil.copy(win_other_dll_path + '/libeay32.dll', os.path.join(output_folder, 'bin'))
        shutil.copy(win_other_dll_path + '/ssleay32.dll', os.path.join(output_folder, 'bin'))
        shutil.copy(win_other_dll_path + '/libmysql.dll', os.path.join(output_folder, 'bin'))

def gen_shell():
    if os.path.exists(output_folder):
        shutil.rmtree(output_folder)
    os.mkdir(output_folder)

    copy_version()

    run_file_name = os.path.join(output_folder, run_shell_file + default_mode_suffix + shell_ext)
    kill_file_name = os.path.join(output_folder, kill_shell_file + default_mode_suffix + shell_ext)

    if os.path.exists(run_file_name):
        os.remove(run_file_name)
    if os.path.exists(kill_file_name):
        os.remove(kill_file_name)

    run_file = open(run_file_name, 'a+')
    kill_file = open(kill_file_name, 'a+')

    if args["onlyzone"]:
        for key in g_proc_config.cluster_procs:
            write_file(run_file, kill_file, g_proc_config.cluster_procs[key])

    for key in g_proc_config.zone_procs:
        write_file(run_file, kill_file, g_proc_config.zone_procs[key])

    run_file.close()
    kill_file.close()

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--mode', type=str, default='release', help="runtime mode, debug/release")
    parser.add_argument('-c', '--channel', type=int, default=100, help="channel id")
    parser.add_argument('-z', '--zone', type=int, default=1, help="zone id")
    parser.add_argument('-l', '--log', type=str, default='1.1', help="log type")
    parser.add_argument('-n', '--service', type=str, default='1.1', help="service type")
    parser.add_argument('-t', '--type', type=int, default='1', help="update type 1 version 2 reload")
    parser.add_argument('-f', '--file', type=str, default='none', help="update file name")
    parser.add_argument('-b', '--branch', type=str, default='develop', help="develop/online/steam")
    parser.add_argument('-o', '--onlyzone', help="only zone servers", action="store_false")

    if is_linux():
        parser.add_argument('-s', '--svn', type=str, help="svn version")
    return vars(parser.parse_args())

# start
args = parse_args()
if args['mode'] == 'release':
    default_mode_suffix = ''  # release下没有后缀

g_proc_config = gcm_conf.load_proc_config(os.path.join(input_folder, 'gcm_proc.xml'))
g_global_config = gcm_global_conf.load_global_config( os.path.join(input_folder, 'global.conf'))
global_conf = dict()
branch_name = args['branch']
global_conf = g_global_config.get_config( args['branch'] )

if args['type'] == 1:
    # version
    print '\nstart to generate shell'
    gen_shell()
    print 'generate shell finished'

    if is_linux() and (args['svn'] is not None):
        print 'start pack RELEASE VERSION'
        now_time = datetime.datetime.now().strftime("%Y%m%d%H%M")
        release_version_name = 'sgame_%s_%s_%s.tar.gz' % (branch_name, args['svn'], now_time)
        tar_cmd = ('tar -zcvf %s %s/*') % (release_version_name, output_folder)
        print tar_cmd
        commands.getoutput(tar_cmd)
        print 'pack RELEASE_VERSION finished'

        # Post to web server
        gcm_http.do_post(global_conf['web_api'], release_version_name)

        # get md5
        (status, output) = commands.getstatusoutput('md5sum %s' % release_version_name)

        # insert into db
        db_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        sql = "INSERT INTO version (version_time, version_name, version_url, version_md5) VALUES ('%s', '%s', '%s', '%s');" % (db_time, release_version_name,  global_conf['web_url'] + release_version_name, output[0: output.find(' ')])
        mysql_db_info = gcm_db.db_info(global_conf['mysql_host'], global_conf['mysql_port'], global_conf['mysql_user'], global_conf['mysql_pwd'], global_conf['mysql_db'])
        gcm_db.insert_mysql_db(mysql_db_info, sql)

        # rm version_name
        rm_cmd = ('rm -rf %s') % (release_version_name)
        commands.getoutput(rm_cmd)    
elif args['type'] == 2:
    # reload
    print '\nstart to update file'

    update_file = args['file']
    (update_path, update_name) = os.path.split(update_file)

    # Post to web server
    gcm_http.do_post(global_conf['web_api'], base_path + update_file)

    (status, output) = commands.getstatusoutput('md5sum %s' % base_path + update_file)

    # insert into db
    db_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    sql = "INSERT INTO file (file_name, file_path, file_url, file_time, file_md5) VALUES ('%s', '%s', '%s', '%s', '%s');" % (update_name, update_path,  global_conf['web_url'] + update_name, db_time, output[0: output.find(' ')])
    mysql_db_info = gcm_db.db_info(global_conf['mysql_host'], global_conf['mysql_port'], global_conf['mysql_user'], global_conf['mysql_pwd'], global_conf['mysql_db'])
    gcm_db.insert_mysql_db(mysql_db_info, sql)

    print 'update file finished'
else:
    print 'error type!'

