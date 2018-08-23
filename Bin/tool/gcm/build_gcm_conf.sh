#!/bin/bash

if [ ! -f conf_input/gcm.conf ];
then
    echo "[ERROR] 'conf_input/gcm.conf' is missing"
    exit -1
fi

cd builder
python gcm_build.py