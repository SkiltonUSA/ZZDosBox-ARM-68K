# ARM64 build environment used by the full-core runtime-clock checks.
FROM docker.io/library/debian@sha256:30bd00fee41ae42067d427c3925dc6f96708d7c518e63f8574a48aca8773d347
RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc-arm-none-eabi=15:15.3.rel1-3 \
    binutils-arm-none-eabi=1:2.47-2+33 \
    picolibc-arm-none-eabi=1.8.12-1 \
    libstdc++-arm-none-eabi-common=15:15.3.rel1-1+33 \
    libstdc++-arm-none-eabi=15:15.3.rel1-1+33 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /work
