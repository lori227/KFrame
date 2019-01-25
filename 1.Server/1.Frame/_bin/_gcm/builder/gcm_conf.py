#!/usr/bin/python
# -*- coding:utf-8 -*-

import sys
import os
import ConfigParser
import gcm_xml

'''
+ cluster_deploy_config
+ zone_deploy_config
'''


class deploy_config:
    def __init__(self):
        self.service_deploy_config = dict()
        self.zone_deploy_config = dict()
        
    def add_service_deploy_config(self, kv_dict):
        self.service_deploy_config = kv_dict

    def add_zone_deploy_config(self, zone_id, kv_dict):
        self.zone_deploy_config[zone_id] = kv_dict

    def get_service_deploy_config(self):
        return self.service_deploy_config

    def get_zone_deploy_config(self, zone_id):
        return self.zone_deploy_config[zone_id]

    def get_zone_list(self):
        return sorted(self.zone_deploy_config.keys())


def load_deploy_config(file_name):
    config = ConfigParser.ConfigParser()
    config.optionxform = str
    config.read(file_name)

    sections = config.sections()
    if len(sections) < 2:
        raise Exception(
            '[ERROR] Sections [SERVICE] and [ZONE #] are expected')

    deploy = deploy_config()
    for section in sections:
        print 'Reading Section [%s]' % (section)

        kv_dict = dict(config.items(section))
        if section == 'SERVICE':
            deploy.add_service_deploy_config(kv_dict)
        elif section.startswith('ZONE'):
            zone_id = int(section[section.find(' ') + 1:])
            deploy.add_zone_deploy_config(zone_id, kv_dict)
        else:
            raise Exception('[ERROR] Unexpected section[%s]' % (section))

    return deploy


class Host:
    '''
    name
    private_ip
    public_ip
    '''

    def __init__(self):
        self.name = ''
        self.private_ip = ''
        self.public_ip = ''


def load_host_config(file_name):
    hosts = dict()
    tree = gcm_xml.read_xml(file_name)
    nodes = gcm_xml.find_nodes(tree, 'Hosts/Host')
    for node in nodes:
        host = Host()
        host.name = node.get('Name')
        host.private_ip = node.get('PrivateIP')
        host.public_ip = node.get('PublicIP')

        if host.name in hosts:
            raise Exception('[ERROR] Duplicate host[%s]' % host.name)

        hosts[host.name] = host

    return hosts


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
