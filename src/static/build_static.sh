#!/usr/bin/env bash

set -xe

cd "$(dirname "$(readlink -f "$0")")"

xxd -i help.txt > help.h
xxd -i guide.txt > guide.h

