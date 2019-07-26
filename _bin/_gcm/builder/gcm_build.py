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

version_file_name = 'version'

base_path = os.path.join('../../')
bin_path = 'bin'
setting_path = 'setting'
config_path = 'config'
startup_path = 'startup'
lib_path = 'lib'
script_path = 'script'

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
    func_id = node.get('ID')
    func_name = node.get('Name')
    func_type = node.get('Type')

    zone_id = args['zone']
    log_type = args['log']
    net_type = args['net']
    service_type = args['service']
    if func_name != 'zone':
        zone_id = 0

    execute_file = default_startup + default_mode_suffix + platform_exe
    run_file.write('echo Starting [%s.%s] server\n' % (func_name, func_type) )
    if is_linux():
        run_file.write('./bin/%s app=%s.%s id=%d.%d.%d service=%s net=%s log=%s \n\n' %
                       (execute_file, func_name, func_type, int(func_id), zone_id, 1, service_type, net_type, log_type ))

        kill_file.write('ps -ef|grep "%s app=%s.%s" | grep -v grep | cut -c 9-15 | xargs kill -9\n\n' %
                        (execute_file, func_name, func_type))
    else:
        run_file.write('start "%s" bin\\%s app=%s.%s id=%d.%d.%d service=%s net=%s log=%s\n\n' %
                       (func_name, execute_file, func_name, func_type, int(func_id), zone_id, 1, service_type, net_type, log_type ))

        kill_file.write('TASKKILL /F /FI "WINDOWTITLE eq %s.%s*"\n\n' % (func_name, func_type))

def copy_setting():
    os.mkdir( os.path.join( output_folder, setting_path ) )
    local_path = base_path + setting_path + '/'

    service = args['service']
    (channel_id, service_type ) = service.split('.')

    for key in g_setting_config.files:
        node = g_setting_config.files[key]
        file_name = node.get('Name')
        file_type = node.get('Type')

        if file_type == "0":
             shutil.copy( local_path + file_name, os.path.join(output_folder, setting_path) )
        else:
            new_file = '%s_%s_%s' % ( channel_id, service_type, file_name )
            if os.path.exists( local_path + new_file ):
                shutil.copy( local_path + new_file, os.path.join(output_folder, setting_path) )
            else:
                new_file = '%s_%s_%s' % ( '0', '0', file_name )
                shutil.copy( local_path + new_file, os.path.join(output_folder, setting_path) )

def copy_version( copybin ):
    if copybin == True:
        if is_linux():
            shutil.copytree(base_path + bin_path + '/linux/' + args['mode'], os.path.join(output_folder, bin_path))
        else:
            shutil.copytree(base_path + bin_path + '/win64/' + args['mode'], os.path.join(output_folder, bin_path))

        shutil.copytree(base_path + startup_path, os.path.join(output_folder, startup_path))

        copy_setting()

        if os.path.exists(base_path + lib_path):
            shutil.copytree(base_path + lib_path, os.path.join(output_folder, lib_path))
    
    if os.path.exists(base_path + config_path):
        shutil.copytree(base_path + config_path, os.path.join(output_folder, config_path))

    if os.path.exists(base_path + script_path):
        shutil.copytree(base_path + script_path, os.path.join(output_folder, script_path))

def gen_shell():
    if os.path.exists(output_folder):
        shutil.rmtree(output_folder)
    if not os.path.exists(output_folder):
        os.mkdir(output_folder)

    copy_version( True )

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
    parser.add_argument('-p', '--project', type=str, default='server', help="project name")
    parser.add_argument('-m', '--mode', type=str, default='release', help="runtime mode, debug/release")
    parser.add_argument('-c', '--service', type=str, default='1.1', help="service type")
    parser.add_argument('-z', '--zone', type=int, default=1, help="zone id")
    parser.add_argument('-l', '--log', type=str, default='1.0', help="log type")
    parser.add_argument('-n', '--net', type=str, default='1', help="net type")
    parser.add_argument('-t', '--type', type=int, default='1', help="update type 1=version 2=file 3=resource 4=plugin")
    parser.add_argument('-f', '--file', type=str, default='none', help="update file name")
    parser.add_argument('-b', '--branch', type=str, default='develop', help="develop/online/steam")
    parser.add_argument('-o', '--onlyzone', help="only zone servers", action="store_false")
    parser.add_argument('-r', '--plugin', type=str, default='none', help="plugin name")
    parser.add_argument('-s', '--svn', type=str, help="svn version")
    parser.add_argument('-v', '--version', type=str, help="build version")
    parser.add_argument('-fn', '--filename', type=str, help="filename")
    parser.add_argument('-md5', '--md5', type=str, help="file md5")

    return vars(parser.parse_args())

def make_version_file(version_name):
    version_file = open(os.path.join(output_folder, version_file_name), 'a+')
    version_file.write("version:" + version_name + "\n")
    version_file.write("svn:" + args['svn'])

# start
args = parse_args()
if args['mode'] == 'release':
    default_mode_suffix = ''  # release下没有后缀

g_proc_config = gcm_conf.load_proc_config(os.path.join(input_folder, 'gcm_proc.xml'))
g_setting_config = gcm_conf.load_setting_config(os.path.join(input_folder, 'gcm_setting.xml'))
g_global_config = gcm_global_conf.load_global_config( os.path.join(input_folder, 'global.conf'))
global_conf = dict()
branch_name = args['branch']
global_conf = g_global_config.get_config( args['branch'] )

if args['type'] == 1:
    # version
    print '\nstart to generate shell'
    gen_shell()
    print 'generate shell finished'

    if (args['svn'] is not None) and (args['version'] is not None):
        print 'start pack RELEASE VERSION'
        release_version_name = '%s_%s_%s.tar.gz' % ( args['project'], branch_name, args['version'])

        make_version_file(args['version'])

        tar_cmd = ('tar -zcvf %s %s/*') % (release_version_name, output_folder)
        print tar_cmd
        commands.getoutput(tar_cmd)
        print 'pack RELEASE_VERSION finished'

        # Post to web server
        gcm_http.do_post(global_conf['web_api'], release_version_name)

        # get md5
        (status, output) = commands.getstatusoutput('md5sum %s' % release_version_name)
        md5output = output[0: output.find(' ')]

        # insert into db
        db_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        sql = "INSERT INTO version (version_time, version_name, version_url, version_md5) VALUES ('%s', '%s', '%s', '%s') on duplicate key update version_time='%s', version_md5='%s';" % (db_time, release_version_name,  global_conf['web_url'] + release_version_name, md5output, db_time, md5output)
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
    md5output = output[0: output.find(' ')]

    # insert into db
    db_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    sql = "INSERT INTO file (file_name, file_path, file_url, file_time, file_md5) VALUES ('%s', '%s', '%s', '%s', '%s') on duplicate key update file_time='%s', file_md5='%s';" % (update_name, update_path,  global_conf['web_url'] + update_name, db_time, md5output, db_time, md5output)
    mysql_db_info = gcm_db.db_info(global_conf['mysql_host'], global_conf['mysql_port'], global_conf['mysql_user'], global_conf['mysql_pwd'], global_conf['mysql_db'])
    gcm_db.insert_mysql_db(mysql_db_info, sql)

    print 'update file finished'
elif args['type'] == 3:
    # resource
    if (args['svn'] is not None) and (args['version'] is not None):
        have_file_name = args['filename'] is not None
        if have_file_name == True:
            release_version_name = args['filename']
        else:
            print '\nstart to generate shell'
            if os.path.exists(output_folder):
                shutil.rmtree(output_folder)
            if not os.path.exists(output_folder):
                os.mkdir(output_folder)
            copy_version( False )
            print 'generate shell finished'
            print 'start pack RELEASE VERSION'
            release_version_name = '%s_%s_%s.tar.gz' % ( args['project'], 'resource', args['version'])
            tar_cmd = ('tar -zcvf %s %s/*') % (release_version_name, output_folder)
            print tar_cmd
            commands.getoutput(tar_cmd)
            print 'pack RELEASE_VERSION finished'

        # Post to web server
        gcm_http.do_post(global_conf['web_api'], release_version_name)

        # get md5
        md5output = args['md5']
        if len(md5output) == 0:
            (status, output) = commands.getstatusoutput('md5sum %s' % release_version_name)
            md5output = output[0: output.find(' ')]

        # insert into db
        db_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        sql = "INSERT INTO resource (resource_time, resource_name, resource_url, resource_md5) VALUES ('%s', '%s', '%s', '%s') on duplicate key update resource_time='%s', resource_md5='%s';" % (db_time, release_version_name,  global_conf['web_url'] + release_version_name, md5output, db_time, md5output)
        mysql_db_info = gcm_db.db_info(global_conf['mysql_host'], global_conf['mysql_port'], global_conf['mysql_user'], global_conf['mysql_pwd'], global_conf['mysql_db'])
        gcm_db.insert_mysql_db(mysql_db_info, sql)

        # rm version_name
        if have_file_name == False:
            rm_cmd = ('rm -rf %s') % (release_version_name)
            commands.getoutput(rm_cmd)   

elif args['type'] == 4:
    # plugin
    print '\nstart to update plugin'

    # Post to web server
    update_file = args['file']
    gcm_http.do_post(global_conf['web_api'], base_path + update_file)

    (status, output) = commands.getstatusoutput('md5sum %s' % base_path + update_file)
    md5output = output[0: output.find(' ')]

    (update_path, update_name) = os.path.split(update_file)

    # insert into db
    file_path = './bin/'
    plugin_name = args['plugin']
    db_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    sql = "INSERT INTO plugin (file_name, file_path, plugin_name, file_url, file_time, file_md5) VALUES ('%s', '%s', '%s', '%s', '%s', '%s') on duplicate key update file_time='%s', file_md5='%s';" % (update_name, file_path, plugin_name, global_conf['web_url'] + update_name, db_time, md5output, db_time, md5output)
    mysql_db_info = gcm_db.db_info(global_conf['mysql_host'], global_conf['mysql_port'], global_conf['mysql_user'], global_conf['mysql_pwd'], global_conf['mysql_db'])
    gcm_db.insert_mysql_db(mysql_db_info, sql)

    print 'update plugin finished'
else:
    print 'error type!'

