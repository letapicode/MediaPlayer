#include "android/AndroidGLVideoOutput.h"
#ifdef __ANDROID__
#include <android/log.h>
#include <android/native_window_jni.h>
#include <cstring>

#define TAG "AndroidGLVideoOutput"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

namespace mediaplayer {

static const char *VERTEX_SHADER = "attribute vec4 aPos;"
                                   "attribute vec2 aTex;"
                                   "varying vec2 vTex;"
                                   "void main(){"
                                   "  gl_Position=aPos;"
                                   "  vTex=aTex;"
                                   "}";

static const char *FRAGMENT_SHADER = "precision mediump float;"
                                     "varying vec2 vTex;"
                                     "uniform sampler2D texY;"
                                     "uniform sampler2D texU;"
                                     "uniform sampler2D texV;"
                                     "void main(){"
                                     "  float y=texture2D(texY,vTex).r;"
                                     "  float u=texture2D(texU,vTex).r-0.5;"
                                     "  float v=texture2D(texV,vTex).r-0.5;"
                                     "  vec3 rgb;"
                                     "  rgb.r=y+1.402*v;"
                                     "  rgb.g=y-0.344*u-0.714*v;"
                                     "  rgb.b=y+1.772*u;"
                                     "  gl_FragColor=vec4(rgb,1.0);"
                                     "}";

static GLuint compileShader(GLenum type, const char *src) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);
  GLint ok;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[256];
    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    LOGE("Shader compile error: %s", log);
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

AndroidGLVideoOutput::AndroidGLVideoOutput() = default;

AndroidGLVideoOutput::~AndroidGLVideoOutput() { shutdown(); }

bool AndroidGLVideoOutput::init(int width, int height) {
  m_width = width;
  m_height = height;
  if (!initEGL())
    return false;
  return initGL();
}

void AndroidGLVideoOutput::shutdown() {
  if (m_program) {
    glDeleteProgram(m_program);
    m_program = 0;
  }
  if (m_texY)
    glDeleteTextures(1, &m_texY);
  if (m_texU)
    glDeleteTextures(1, &m_texU);
  if (m_texV)
    glDeleteTextures(1, &m_texV);
  destroyEGL();
}

void AndroidGLVideoOutput::setWindow(ANativeWindow *window) {
  if (m_window == window)
    return;
  shutdown();
  m_window = window;
  if (m_window)
    init(m_width, m_height);
}

bool AndroidGLVideoOutput::initEGL() {
  if (!m_window)
    return false;
  m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (m_display == EGL_NO_DISPLAY)
    return false;
  if (!eglInitialize(m_display, nullptr, nullptr))
    return false;
  EGLint cfgAttrs[] = {EGL_RENDERABLE_TYPE,
                       EGL_OPENGL_ES2_BIT,
                       EGL_SURFACE_TYPE,
                       EGL_WINDOW_BIT,
                       EGL_RED_SIZE,
                       8,
                       EGL_GREEN_SIZE,
                       8,
                       EGL_BLUE_SIZE,
                       8,
                       EGL_NONE};
  EGLConfig config;
  EGLint num;
  if (!eglChooseConfig(m_display, cfgAttrs, &config, 1, &num) || num < 1)
    return false;
  m_surface = eglCreateWindowSurface(m_display, config, m_window, nullptr);
  EGLint ctxAttrs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
  m_context = eglCreateContext(m_display, config, EGL_NO_CONTEXT, ctxAttrs);
  if (m_surface == EGL_NO_SURFACE || m_context == EGL_NO_CONTEXT)
    return false;
  if (!eglMakeCurrent(m_display, m_surface, m_surface, m_context))
    return false;
  return true;
}

bool AndroidGLVideoOutput::initGL() {
  GLuint vs = compileShader(GL_VERTEX_SHADER, VERTEX_SHADER);
  GLuint fs = compileShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
  if (!vs || !fs)
    return false;
  m_program = glCreateProgram();
  glAttachShader(m_program, vs);
  glAttachShader(m_program, fs);
  glLinkProgram(m_program);
  glDeleteShader(vs);
  glDeleteShader(fs);
  GLint linked;
  glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
  if (!linked) {
    char log[256];
    glGetProgramInfoLog(m_program, sizeof(log), nullptr, log);
    LOGE("Program link error: %s", log);
    glDeleteProgram(m_program);
    m_program = 0;
    return false;
  }
  glGenTextures(1, &m_texY);
  glGenTextures(1, &m_texU);
  glGenTextures(1, &m_texV);
  return true;
}

void AndroidGLVideoOutput::destroyEGL() {
  if (m_display != EGL_NO_DISPLAY) {
    eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  }
  if (m_context != EGL_NO_CONTEXT) {
    eglDestroyContext(m_display, m_context);
    m_context = EGL_NO_CONTEXT;
  }
  if (m_surface != EGL_NO_SURFACE) {
    eglDestroySurface(m_display, m_surface);
    m_surface = EGL_NO_SURFACE;
  }
  if (m_display != EGL_NO_DISPLAY) {
    eglTerminate(m_display);
    m_display = EGL_NO_DISPLAY;
  }
}

void AndroidGLVideoOutput::uploadTextures(const uint8_t *yuv420p) {
  const uint8_t *yPlane = yuv420p;
  const uint8_t *uPlane = yPlane + m_width * m_height;
  const uint8_t *vPlane = uPlane + (m_width / 2) * (m_height / 2);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texY);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_width, m_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
               yPlane);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_texU);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_width / 2, m_height / 2, 0, GL_LUMINANCE,
               GL_UNSIGNED_BYTE, uPlane);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_texV);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_width / 2, m_height / 2, 0, GL_LUMINANCE,
               GL_UNSIGNED_BYTE, vPlane);
}

void AndroidGLVideoOutput::displayFrame(const uint8_t *yuv420p, int) {
  if (!m_window || m_display == EGL_NO_DISPLAY)
    return;
  eglMakeCurrent(m_display, m_surface, m_surface, m_context);
  uploadTextures(yuv420p);
  glViewport(0, 0, m_width, m_height);
  glUseProgram(m_program);
  GLint posLoc = glGetAttribLocation(m_program, "aPos");
  GLint texLoc = glGetAttribLocation(m_program, "aTex");
  GLfloat vertices[] = {-1.f, -1.f, 0.f, 0.f, 1.f, 1.f, -1.f, 0.f, 1.f, 1.f,
                        -1.f, 1.f,  0.f, 0.f, 0.f, 1.f, 1.f,  0.f, 1.f, 0.f};
  glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), vertices);
  glEnableVertexAttribArray(posLoc);
  glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), vertices + 3);
  glEnableVertexAttribArray(texLoc);
  glUniform1i(glGetUniformLocation(m_program, "texY"), 0);
  glUniform1i(glGetUniformLocation(m_program, "texU"), 1);
  glUniform1i(glGetUniformLocation(m_program, "texV"), 2);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  eglSwapBuffers(m_display, m_surface);
}

} // namespace mediaplayer

extern "C" {

JNIEXPORT jlong JNICALL Java_com_example_mediaplayer_GLVideoOutput_nativeCreate(JNIEnv *, jobject) {
  return reinterpret_cast<jlong>(new mediaplayer::AndroidGLVideoOutput());
}

JNIEXPORT void JNICALL Java_com_example_mediaplayer_GLVideoOutput_nativeDestroy(JNIEnv *, jobject,
                                                                                jlong handle) {
  auto *output = reinterpret_cast<mediaplayer::AndroidGLVideoOutput *>(handle);
  delete output;
}

JNIEXPORT void JNICALL Java_com_example_mediaplayer_GLVideoOutput_nativeSetSurface(
    JNIEnv *env, jobject, jlong handle, jobject surface) {
  auto *output = reinterpret_cast<mediaplayer::AndroidGLVideoOutput *>(handle);
  ANativeWindow *window = nullptr;
  if (surface)
    window = ANativeWindow_fromSurface(env, surface);
  output->setWindow(window);
  if (window)
    ANativeWindow_release(window);
}

JNIEXPORT void JNICALL Java_com_example_mediaplayer_GLVideoOutput_nativeRender(
    JNIEnv *env, jobject, jlong handle, jbyteArray data, jint width, jint height) {
  auto *output = reinterpret_cast<mediaplayer::AndroidGLVideoOutput *>(handle);
  jsize size = env->GetArrayLength(data);
  if (size < width * height * 3 / 2)
    return;
  jbyte *ptr = env->GetByteArrayElements(data, nullptr);
  output->init(width, height);
  output->displayFrame(reinterpret_cast<uint8_t *>(ptr), width);
  env->ReleaseByteArrayElements(data, ptr, JNI_ABORT);
}

} // extern "C"
#endif // __ANDROID__
