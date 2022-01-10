#/bin/bash
ffmpeg -y -f v4l2 -video_size 1280x720 -i /dev/video0 \
       -r 0.2 -qscale:v 2 -update 1 -vframes 1 ./$1.jpg
