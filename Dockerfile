FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    cmake ninja-build \
    g++ gcc \
    libgstreamer1.0 \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0 \
    libgstreamer-plugins-base1.0-dev \
    libgstreamer-plugins-good1.0 \
    libgstreamer-plugins-good1.0-dev \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav

ARG WITH_VA
RUN echo "WITH_VA=$WITH_VA"
RUN [ -n "$WITH_VA" ] \
    && apt-get update \
    && apt-get install -y \
        gstreamer1.0-vaapi \
        vainfo \
    || true

VOLUME /data

RUN mkdir -pv /test && mkdir -pv /test-build
ADD main.cpp /test
ADD CMakeLists.txt /test

WORKDIR /test-build

RUN cmake -GNinja ../test && ninja
CMD ./experiment /data/video.mp4
