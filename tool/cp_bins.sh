#!/bin/bash

cp ../build/bootloader/bootloader.bin ../release_bin/
cp ../build/family_mruby.bin ../release_bin/
cp ../build/partitions.bin ../release_bin/
cp ./spiffs_image ../release_bin/
