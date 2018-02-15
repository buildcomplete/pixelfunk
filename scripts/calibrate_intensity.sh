#!/bin/bash

led_offset=10

for i in `seq 1 20`;
do
	let idx=i+led_offset;
	echo LED $idx 1 0 0 > /dev/ttyUSB0
done  
