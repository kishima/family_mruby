#!/bin/bash

./mkspiffs -c ./spiffs_data -b 4096 -p 256 -s 0x1F0000 spiffs_image.bin

cp ../build/bootloader/bootloader.bin ../release_bin/
cp ../build/family_mruby.bin ../release_bin/
cp ../build/partitions.bin ../release_bin/
cp ./spiffs_image.bin ../release_bin/

cd ../release_bin
zip family_mruby_bin.zip ./*.bin

