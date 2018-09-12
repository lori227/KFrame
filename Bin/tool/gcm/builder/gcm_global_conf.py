#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys
import os
import ConfigParser

class global_config:
    def __init__(self):
        self.internal_config = dict()
        self.public_config = dict()
        self.develop_config = dict()

    def add_internal_config(self, kv_dict):
        self.internal_config = kv_dict

    def add_public_config(self, kv_dict):
        self.public_config = kv_dict
    
    def add_develop_config(self, kv_dict):
        self.develop_config = kv_dict

    def get_internal_config(self):
        return self.internal_config

    def get_public_config(self):
        return self.public_config

    def get_develop_config(self):
        return self.develop_config

def load_global_config(file_name):
    config = ConfigParser.ConfigParser()
    config.optionxform = str
    config.read(file_name)
    
    sections = config.sections()
    if len(sections) < 3:
        raise Exception(
            '[ERROR] Sections [DEVELOP] [INTERNAL] [PUBLIC] are expected')
    
    conf_config = global_config()
    for section in sections:
        print 'Reading section [%s]' % (section)

        kv_dict = dict(config.items(section))
        if section == 'INTERNAL':
            conf_config.add_internal_config(kv_dict)
        elif section == 'PUBLIC':
            conf_config.add_public_config(kv_dict)
        elif section == 'DEVELOP':
            conf_config.add_develop_config(kv_dict)           
        else:
            raise Exception('[ERROR] Unexpected section[%s]' % (section))

    return conf_config