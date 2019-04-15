#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys
import os
import ConfigParser

class global_config:
    def __init__(self):
        self._configs = dict()

    def add_config(self, name, kv_dict):
        self._configs[name] = kv_dict

    def get_config(self, name):
        return self._configs[name]

def load_global_config(file_name):
    config = ConfigParser.ConfigParser()
    config.optionxform = str
    config.read(file_name)
    
    conf_config = global_config()
    sections = config.sections()
    for section in sections:
        print 'Reading section [%s]' % (section)

        kv_dict = dict(config.items(section))
        conf_config.add_config(section, kv_dict)

    return conf_config