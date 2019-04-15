#!/usr/bin/python
# -*- coding:utf-8 -*-

import sys
import os
import ConfigParser
import gcm_xml

class ProcConfig:
    def __init__(self):
        self.cluster_procs = dict()
        self.zone_procs = dict()


def load_proc_config(file_name):
    proc_config = ProcConfig()
    tree = gcm_xml.read_xml(file_name)
    cluster_nodes = gcm_xml.find_nodes(tree, 'cluster')
    cluster_proc_nodes = gcm_xml.find_nodes(tree, 'cluster/Proc')
    for node in cluster_proc_nodes:
        proc_config.cluster_procs[node.get('ID')] = node

    zone_proc_nodes = gcm_xml.find_nodes(tree, 'cluster/zone/Proc')
    for node in zone_proc_nodes:
        proc_config.zone_procs[node.get('ID')] = node

    return proc_config

#########################################################
#########################################################
class SettingConfig:
    def __init__(self):
        self.files = dict()

def load_setting_config(file_name):
    setting_config = SettingConfig()
    tree = gcm_xml.read_xml(file_name)
    setting_nodes = gcm_xml.find_nodes(tree, 'setting')
    file_nodes = gcm_xml.find_nodes(tree, 'setting/file')
    for node in file_nodes:
        name = node.get('Name')
        setting_config.files[name] = node

    return setting_config