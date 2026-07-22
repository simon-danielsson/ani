#!/usr/bin/env bash

set -xe

cd "$(dirname "$(readlink -f "$0")")"

./bedh.py help.txt
./bedh.py guide.txt

