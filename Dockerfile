
FROM --platform=linux/amd64 ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    nasm \
    binutils \
    gcc \
    libc6-dev \
    make \
    gdb \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work

CMD ["bash"]
