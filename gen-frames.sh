#!/bin/bash

set -e

if [ -f frames/video.mp4 -a -f frames/video.mkv ] ; then
    echo "Skip generation - files exist"
    exit
fi

mkdir -pv frames

echo "Generating frames..."

seq 0 99 | xargs -n 1 printf '%03d\n' | xargs -n 1 -P 8 -I {} \
convert \
    -size 320x240 \
    xc:lightblue \
    -pointsize 50 \
    -fill green  \
    -stroke black  \
    -gravity center \
    -draw "text 0,0 'FRAME {}'" \
    frames/frame_{}.png

echo "Encoding..."

gst-launch-1.0 \
    multifilesrc location=frames/frame_%03d.png caps="image/png,framerate=30/1" \
    ! pngdec \
    ! videoconvert \
    ! videorate \
    ! x264enc \
    ! matroskamux \
    ! filesink location=frames/video.mkv

gst-launch-1.0 \
    multifilesrc location=frames/frame_%03d.png caps="image/png,framerate=30/1" \
    ! pngdec \
    ! videoconvert \
    ! videorate \
    ! x264enc \
    ! qtmux \
    ! filesink location=frames/video.mp4

echo "Cleanup..."

rm frames/*.png

echo "Done!"

