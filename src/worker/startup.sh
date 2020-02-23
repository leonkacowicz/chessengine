#!/bin/bash
cd /home/ubuntu || (echo "Could not cd to /home/ubuntu" && exit 1)
sudo apt update
sudo dpkg-reconfigure -f noninteractive unattended-upgrades
sudo apt upgrade -y
sudo apt install -y jq python3 python3-pip
pip3 install boto3 --upgrade
pip3 install boto3_type_annotations --upgrade
mkdir -p worker_template
aws s3 sync s3://leonkacowicz/chess/worker/ worker_template/
chmod +x worker_template/*
for CPU in $(awk '/^processor/{print $3}' < /proc/cpuinfo | head -n -1); do
    mkdir -p "worker_$CPU"
    cp -r worker_template/* "worker_$CPU"
    (
        cd "worker_$CPU" || exit 1
        python3 ./worker.py &
    )
done
#python3 ./worker.py
