# DevOps & Infrastructure

This directory contains container definitions and helper scripts for building MediaPlayer.

## Qt build container

`Dockerfile.qt` provides an Ubuntu image with the Qt 6 SDK installed. Use it when you want to compile the desktop application without installing Qt locally.

### Build the image

```bash
docker build -f Dockerfile.qt -t mediaplayer-qt .
```

### Build the desktop app inside the container

Running the image drops you in a shell where the project has already been configured and built with CMake. The resulting binary lives in `/build/mediaplayer_desktop_app`.

```bash
docker run --rm -it mediaplayer-qt
```

To launch the GUI on a Linux host using X11 forwarding:

```bash
docker run --rm -it \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    mediaplayer-qt /build/mediaplayer_desktop_app
```
