#ifndef MEDIAPLAYER_ANDROIDGLVIDEOOUTPUT_H
#define MEDIAPLAYER_ANDROIDGLVIDEOOUTPUT_H

#ifdef __ANDROID__

#include "mediaplayer/VideoOutput.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/native_window.h>

namespace mediaplayer {

class AndroidGLVideoOutput : public VideoOutput {
public:
  AndroidGLVideoOutput();
  ~AndroidGLVideoOutput() override;

  bool init(int width, int height) override;
  void shutdown() override;
  void displayFrame(const uint8_t *yuv420p, int linesize) override;

  void setWindow(ANativeWindow *window);

private:
  bool initEGL();
  bool initGL();
  void destroyEGL();
  void uploadTextures(const uint8_t *yuv420p);

  ANativeWindow *m_window{nullptr};
  EGLDisplay m_display{EGL_NO_DISPLAY};
  EGLSurface m_surface{EGL_NO_SURFACE};
  EGLContext m_context{EGL_NO_CONTEXT};
  GLuint m_program{0};
  GLuint m_texY{0};
  GLuint m_texU{0};
  GLuint m_texV{0};
  int m_width{0};
  int m_height{0};
};

} // namespace mediaplayer

extern "C" {
#include <jni.h>

JNIEXPORT jlong JNICALL Java_com_example_mediaplayer_GLVideoOutput_nativeCreate(JNIEnv *, jobject);
JNIEXPORT void JNICALL Java_com_example_mediaplayer_GLVideoOutput_nativeDestroy(JNIEnv *, jobject,
                                                                                jlong);
JNIEXPORT void JNICALL Java_com_example_mediaplayer_GLVideoOutput_nativeSetSurface(JNIEnv *,
                                                                                   jobject, jlong,
                                                                                   jobject);
JNIEXPORT void JNICALL Java_com_example_mediaplayer_GLVideoOutput_nativeRender(JNIEnv *, jobject,
                                                                               jlong, jbyteArray,
                                                                               jint, jint);
}

#endif // __ANDROID__
#endif // MEDIAPLAYER_ANDROIDGLVIDEOOUTPUT_H
