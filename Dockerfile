FROM ubuntu:22.04

RUN apt-get update && apt-get install -y gcc make binutils libc6-dev
WORKDIR /workspace