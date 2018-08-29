#!/usr/bin/python
# -*- coding: UTF-8 -*-

# author: Nick Yang
# date: 2018/8/8
# last update time: 2018/8/21
# version: 0.3
# changelog:
#   1.打包版本
#   2.增加只在linux下打包版本
#   3.增加开启服务器的shell
#   4.startup配置和其他配置分开


import os
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
import gcm_scp
import gcm_db
import gcm_http

input_folder = '../conf_input'
output_folder = '../conf_output'

default_version_api = 'http://version.leiwu.com/upload.php'
default_version_url = 'http://version.leiwu.com/upload/'
default_startup = 'KFStartup'
default_mode_suffix = 'd'  # debug release后缀


def is_linux():
    if platform.system() == 'Windows':
        return False
    else:
        return True


platform_path = ''  # copy path
platform_exe = ''  # executable suffix
platform_dll = ''  # dynamic library suffix
runtime_mode = ''  # debug release
if is_linux():
    platform_path = 'linux'
    platform_exe = ''
    platform_dll = '.so'
else:
    platform_path = 'win64'
    platform_exe = '.exe'
    platform_dll = '.dll'

bin_path = os.path.join('../../../bin')
plugin_path = bin_path
setting_path = os.path.join('../../../setting')
config_path = os.path.join('../../../config')
startup_path = os.path.join('../../../startup')
win_other_dll_path = os.path.join('../../../../Server/_lib/win64/3rd')


# 获取本机ip
def get_local_ip():
    hostname = socket.getfqdn(socket.gethostname())
    host = socket.gethostbyname(hostname)
    return host


def create_dir(dir):
    os.makedirs(dir)


def get_real_src_path(is_execute=False, is_dll=False, dll_name='', is_setting=False, is_config=False, is_startup=False, conf_name=''):
    if not is_execute and not is_dll and not is_setting and not is_config and not is_startup:
        raise Exception('get_real_src_path failed')

    if is_dll and dll_name == '':
        raise Exception('get_real_src_path failed, dll name is empty')

    if is_execute:
        return os.path.join(bin_path, default_startup + default_mode_suffix + platform_exe)
    elif is_dll:
        return os.path.join(plugin_path, dll_name + platform_dll)
    elif is_setting:
        return os.path.join(setting_path, conf_name)
    elif is_config:
        return os.path.join(config_path, conf_name)
    elif is_startup:
        return os.path.join(startup_path, conf_name)
    else:
        raise Exception('[ERROR] cannot get real path')


def copy_plugin_and_setting(tree, config_folder, plugin_folder, setting_folder, startup_folder):
    plugin_nodes = gcm_xml.find_nodes(tree, 'Plugins/Plugin')
    for node in plugin_nodes:
        dll_name = node.get('Name') + default_mode_suffix
        shutil.copy(get_real_src_path(
            is_dll=True, dll_name=dll_name), plugin_folder)
        config = node.get('Config')
        if len(config) > 0:
            config_name = config[config.rfind('/') + 1:]
            if config.find('setting') != -1:
                # copy to setting folder
                shutil.copy(get_real_src_path(
                    is_setting=True, conf_name=config_name), setting_folder)
            elif config.find('config') != -1:
                # copy to config folder
                shutil.copy(get_real_src_path(
                    is_config=True, conf_name=config_name), config_folder)
            elif config.find('startup') != -1:
                #copy startup files to setting folder
                shutil.copy(get_real_src_path(
                    is_startup=True, conf_name=config_name), startup_folder)
            else:
                raise Exception(
                    '[ERROR] invalid filed, file = %s Config = %s' % (tree, config))

#生成shell文件bat/sh
def generate_shell_file(folder, startup_name, proc_name):
    func_id = 0
    zone_id = args['zone']
    if proc_name in g_proc_config.cluster_procs:
        func_id = g_proc_config.cluster_procs[proc_name].get('FuncID')
        zone_id = 0
    elif proc_name in g_proc_config.zone_procs:
        func_id = g_proc_config.zone_procs[proc_name].get('FuncID')
    else:
        raise Exception('[ERROR] %s is not a normal process' % proc_name)

    if is_linux():
        run_file = open(folder + '/run' + default_mode_suffix + '.sh', 'a+')
        run_file.write('echo Starting ' + proc_name + ' Server\n')
        run_file.write('./%s appid=%d.%d.%d.%d log=1 startup=./startup/%s.startup\n\n' % (startup_name, args['channel'], zone_id, int(func_id), 1, proc_name))
        run_file.close()

        kill_file = open(folder + '/kill' + default_mode_suffix + '.sh', 'a+')
        kill_file.write('echo Killing ' + proc_name + ' Server\n')
        kill_file.write('kill -9 $(pidof %s)' % proc_name)
        kill_file.close()
    else:  
        run_file = open(folder + '/run' + default_mode_suffix + '.bat', 'a+')
        run_file.write('echo Starting ' + proc_name + ' Server\n')
        run_file.write('start "%s" %s appid=%d.%d.%d.%d log=1 startup=./startup/%s.startup\n\n' % (proc_name, startup_name, args['channel'], zone_id, int(func_id), 1, proc_name))
        run_file.close()

        kill_file = open(folder + '/kill' + default_mode_suffix + '.bat', 'a+')
        kill_file.write('@echo off\n')
        kill_file.write('echo Killing ' + proc_name + ' Server\n')
        kill_file.write('TASKKILL /F /FI "WINDOWTITLE eq %s*"' % proc_name)
        kill_file.close()

# 拷贝其他windows依赖的dll文件
def copy_windows_other_dlls(out_folder):
    if is_linux():
        return
    else:
        shutil.copy(win_other_dll_path + '/libeay32.dll', out_folder)
        shutil.copy(win_other_dll_path + '/ssleay32.dll', out_folder)
        shutil.copy(win_other_dll_path + '/libmysql.dll', out_folder)

# 拷贝进程的文件
def copy_proc_files(node):
    # 1.创建文件夹(根据proc配置)
    full_name = node.get('FuncName')
    func_name = (full_name if full_name.find('.') == -
                 1 else full_name[0: (full_name.find('.'))])
    out_folder = os.path.join(output_folder, func_name)
    out_config_folder = os.path.join(output_folder, func_name, 'config')
    out_plugin_folder = os.path.join(output_folder, func_name, 'plugin')
    out_setting_folder = os.path.join(output_folder, func_name, 'setting')
    out_startup_folder = os.path.join(output_folder, func_name, 'startup')
    if not os.path.exists(out_folder):
        create_dir(out_folder)
        create_dir(out_config_folder)
        create_dir(out_plugin_folder)
        create_dir(out_setting_folder)
        create_dir(out_startup_folder)

    # 2.拷贝KFStartup(d).exe
    startup_src_path = get_real_src_path(is_execute=True)
    startup_name = node.get('ProcName') + default_mode_suffix + platform_exe
    shutil.copy(startup_src_path, out_folder + '/' + startup_name)

    # 3.拷贝dll/so(根据*.startup文件)
    tree = gcm_xml.read_xml(os.path.join(startup_path, full_name + '.startup'))
    plugins_nodes = gcm_xml.find_nodes(tree, 'Plugins')

    common = plugins_nodes[0].get('Common')
    if (common != None) and (len(common) > 0):
        common = common[common.rfind('/') + 1:]
        common_tree = gcm_xml.read_xml(os.path.join(startup_path, common))
        copy_plugin_and_setting(
            common_tree, out_config_folder, out_plugin_folder, out_setting_folder, out_startup_folder)
        shutil.copy(os.path.join(startup_path, common), out_startup_folder)

    copy_plugin_and_setting(tree, out_config_folder,
                            out_plugin_folder, out_setting_folder, out_startup_folder)
    shutil.copy(os.path.join(startup_path, full_name +
                             '.startup'), out_startup_folder)

    generate_shell_file(out_folder, startup_name, full_name)
    copy_windows_other_dlls(out_folder)

def copy_files():
    print 'start to copy all necessary files'

    for key in g_proc_config.cluster_procs:
        copy_proc_files(g_proc_config.cluster_procs[key])

    for key in g_proc_config.zone_procs:
        copy_proc_files(g_proc_config.zone_procs[key])

    print 'copy all necessary files finished'


def gen_configuration():
    if os.path.exists(output_folder):
        shutil.rmtree(output_folder)
    os.mkdir(output_folder)

    copy_files()


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--mode', type=str, default='release', help="runtime mode, debug/release")
    parser.add_argument('-c', '--channel', type=int, default=100, help="channel id")
    parser.add_argument('-z', '--zone', type=int, default=1, help="zone id")
    if is_linux():
        parser.add_argument('-s', '--svn', type=int, help="svn version")
    return vars(parser.parse_args())

# start
args = parse_args()
if args['mode'] == 'release':
    default_mode_suffix = ''  # release下没有后缀

g_deploy_config = gcm_conf.load_deploy_config(
    os.path.join(input_folder, 'gcm.conf'))
g_host_config = gcm_conf.load_host_config(
    os.path.join(input_folder, 'gcm_host.xml'))
g_proc_config = gcm_conf.load_proc_config(
    os.path.join(input_folder, 'gcm_proc.xml'))

print '\nstart to generate configurations'
gen_configuration()
print 'generate configurations finished'

if is_linux() and (args['svn'] is not None):
    print 'start pack RELEASE VERSION'
    release_version_name = 'sgame_svn_' + str(args['svn']) + '_' + datetime.datetime.now().strftime("%Y%m%d%H%M") + '.tar.gz'
    tar_cmd = 'tar -zcvf ' + release_version_name + ' ' + output_folder + '/*'
    print tar_cmd
    commands.getoutput(tar_cmd)
    print 'pack RELEASE_VERSION finished'

    # scp to version repo
    #gcm_scp.ssh_scp_put(default_version_ip, 22, 'root', '123456#',
    #                    release_version_name, '/home/sgversion/' + release_version_name)

    # Post to web server
    gcm_http.do_post(default_version_api, release_version_name)

    # get md5
    (status, output) = commands.getstatusoutput('md5sum %s' % release_version_name)

    # insert into db(因暂时没有web管理，所以先用ssh的地址)
    gcm_db.insert_mysql_db("INSERT INTO version (version_time, version_name, version_url, version_md5) VALUES ('%s', '%s', '%s', '%s');" %
                        (datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"), release_version_name, default_version_url + release_version_name, output[0: output.find(' ')]))
