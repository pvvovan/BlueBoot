mount -t tmpfs -o size=512m tmpfs ./ramdisk/
ffmpeg -y -f v4l2 -i /dev/video0 -update 1 -r 30 ./ramdisk/output.jpg
