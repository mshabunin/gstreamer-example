#!/bin/bash

set -e
set -x

./gen-frames.sh

TAG=gst-experiment

    # --build-arg WITH_VA=1 \
docker build \
    -t $TAG \
    .

docker run -it \
    --volume $(readlink -f frames):/data:ro \
    --device /dev/dri/renderD128 \
    $TAG \
    ./experiment /data/video.mp4

docker run -it \
    --volume $(readlink -f frames):/data:ro \
    --device /dev/dri/renderD128 \
    $TAG \
    ./experiment /data/video.mkv

docker run -it \
    --volume $(readlink -f frames):/data:ro \
    --device /dev/dri/renderD128 \
    $TAG \
    ./experiment /data/video.avi


docker run -it \
    --volume $(readlink -f frames):/data:ro \
    --device /dev/dri/renderD128 \
    $TAG \
    ./experiment /data/video8.webm

docker run -it \
    --volume $(readlink -f frames):/data:ro \
    --device /dev/dri/renderD128 \
    $TAG \
    ./experiment /data/video9.webm

