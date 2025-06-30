# Video Output Implementation Tasks

This document expands on the Video Output section in [Tasks.MD](../Tasks.MD). Each bullet corresponds to a focused development unit so agents can work in isolation.

## 19. Abstract Video Output Interface

- Define an abstract class `VideoOutput` with methods:
  - `bool init(int width, int height)` – prepare the renderer with the given size.
  - `void shutdown()` – release all resources.
  - `void displayFrame(const uint8_t *rgba, int linesize)` – present a frame already converted to RGBA.
  - `void displayFrame(const VideoFrame &frame)` – expects YUV plane data and is typically called by decoders.

## 20. Video Rendering – OpenGL (Desktop)

1. **Create an OpenGL context** using GLFW (or accept one from the UI). Initialize GLEW if necessary.
2. **Upload YUV planes as textures** and write a GLSL shader that converts YUV to RGB.
3. **Render a full-screen quad** with the shader and swap buffers every frame.

## 21. Video Rendering – DirectX (Windows optional)

1. **Set up a D3D11 device and swap chain** bound to the target window.
2. **Create textures for Y, U and V planes** and a pixel shader performing YUV→RGB conversion.
3. **Draw the frame** to a quad and present via the swap chain.

## 22. Video Rendering – Metal (macOS)

1. **Create a `CAMetalLayer` or `MTKView`** and obtain a `MTLDevice` and command queue.
2. **Convert YUV to RGB** using a Metal shader or `vImageConvert` and render to the drawable.

## 23. Frame Renderer – Android (OpenGL ES)

1. **Receive an `ANativeWindow` from Java** and create an EGL context.
2. **Compile an OpenGL ES program** for YUV→RGB conversion and upload frame textures each render call.
3. **Expose JNI hooks** so the Java layer can pass surfaces and drive the renderer.

## 24. Frame Renderer – iOS (Metal/GL ES)

1. **Use `MTKView` or `GLKView`** to host the graphics context on iOS.
2. **Render YUV frames** with either Metal shaders or OpenGL ES, depending on the chosen approach.

## 25. Video Output Integration

1. **Connect decoded frames to the renderer** via a thread‑safe queue or double buffer inside `MediaPlayer::videoLoop`.
2. **Handle shutdown and pausing** so rendering stops cleanly when playback is stopped or the window closes.
