#!/bin/sh
for i in /lib/modules/*/extra/*.ko; do
	name=$(basename $i .ko)
	echo "loading $i ($name)..."
	rmmod $name.ko
	modprobe $name
done
