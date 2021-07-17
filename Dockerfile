FROM ubuntu:latest
LABEL maintainer="acedron <acedrons@yahoo.co.jp>"

WORKDIR /app

COPY . .

RUN apt-get -y update
RUN apt-get -y install cmake gcc libx11-dev libxi-dev
RUN cmake -B build
RUN cmake --build build
RUN cmake --install build