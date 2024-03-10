# Build image
# Run `docker build --target=<debug/release/all> --output=bin .`
# Binaries will appear in ./bin
FROM docker.io/devkitpro/devkitppc as dkp

FROM docker.io/debian:latest as build-env
WORKDIR /src
RUN apt-get update && apt-get install -y build-essential pkg-config scons libx11-dev libxcursor-dev libxinerama-dev libgl1-mesa-dev libglu-dev libasound2-dev libpulse-dev libfreetype6-dev libssl-dev libudev-dev libxi-dev libxrandr-dev
COPY --from=dkp /opt/devkitpro /opt/devkitpro
ENV DEVKITPRO=/opt/devkitpro
COPY . /src

FROM build-env AS debug-build
RUN scons -Q verbose=yes platform=wii target=debug

FROM scratch AS debug
COPY --from=debug-build /src/bin/* /

FROM build-env AS release-build
RUN scons -Q verbose=yes platform=wii target=release

FROM scratch AS release
COPY --from=release-build /src/bin/* /

FROM scratch AS all
COPY --from=debug /* /
COPY --from=release /* /