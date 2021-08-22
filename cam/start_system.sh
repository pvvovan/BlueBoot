#!/bin/bash
# Start corresponding apps for blueboot to operate via phone

ramdiskpath=$(df | grep "cam/ramdisk")
rl=${#ramdiskpath}

if [[ ${rl} -gt 10 ]]
then
	./jpgSrv &
	../mover/move &
	ffmpeg -y -f v4l2 -i /dev/video0 -update 1 -r 5 ./ramdisk/output.jpg
	kill %1
	kill %2
else
	tput setaf 1; echo "ERROR: no ramdisk!"
fi

