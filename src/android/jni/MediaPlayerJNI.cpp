#include "android/AndroidGLVideoOutput.h"
#include "mediaplayer/MediaPlayer.h"
#include <android/native_window_jni.h>
#include <jni.h>
#include <memory>

using mediaplayer::MediaPlayer;

static std::unique_ptr<MediaPlayer> g_player;
static jobject g_callback = nullptr;
static JavaVM *g_vm = nullptr;

static void setupCallbacks();

extern "C" jint JNI_OnLoad(JavaVM *vm, void *) {
  g_vm = vm;
  return JNI_VERSION_1_6;
}

extern "C" void Java_com_example_mediaplayer_MediaPlayerNative_nativeSetListener(JNIEnv *env,
                                                                                 jclass,
                                                                                 jobject listener) {
  if (g_callback) {
    env->DeleteGlobalRef(g_callback);
    g_callback = nullptr;
  }
  if (listener) {
    g_callback = env->NewGlobalRef(listener);
    if (g_player)
      setupCallbacks();
  }
}

extern "C" void Java_com_example_mediaplayer_MediaPlayerNative_nativePlay(JNIEnv *, jclass) {
  if (g_player)
    g_player->play();
}

extern "C" void Java_com_example_mediaplayer_MediaPlayerNative_nativePause(JNIEnv *, jclass) {
  if (g_player)
    g_player->pause();
}

extern "C" jboolean Java_com_example_mediaplayer_MediaPlayerNative_nativeOpen(JNIEnv *env, jclass,
                                                                              jstring path) {
  if (!g_player)
    g_player = std::make_unique<MediaPlayer>();
  const char *cpath = env->GetStringUTFChars(path, nullptr);
  bool ok = g_player->open(cpath);
  env->ReleaseStringUTFChars(path, cpath);
  setupCallbacks();
  return ok ? JNI_TRUE : JNI_FALSE;
}

extern "C" void Java_com_example_mediaplayer_MediaPlayerNative_nativeStop(JNIEnv *, jclass) {
  if (g_player)
    g_player->stop();
}

extern "C" void Java_com_example_mediaplayer_MediaPlayerNative_nativeSeek(JNIEnv *, jclass,
                                                                          jdouble pos) {
  if (g_player)
    g_player->seek(pos);
}

extern "C" jobjectArray Java_com_example_mediaplayer_MediaPlayerNative_nativeListMedia(JNIEnv *env,
                                                                                       jclass) {
  if (!g_player)
    g_player = std::make_unique<MediaPlayer>();
  auto items = g_player->allMedia();
  jobjectArray arr = env->NewObjectArray(items.size(), env->FindClass("java/lang/String"), nullptr);
  for (size_t i = 0; i < items.size(); ++i) {
    env->SetObjectArrayElement(arr, i, env->NewStringUTF(items[i].path.c_str()));
  }
  return arr;
}

extern "C" void Java_com_example_mediaplayer_MediaPlayerNative_nativeSetSurface(JNIEnv *env, jclass,
                                                                                jobject surface) {
  if (!g_player) {
    g_player = std::make_unique<MediaPlayer>();
    setupCallbacks();
  }

  if (surface) {
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    auto output = std::make_unique<mediaplayer::AndroidGLVideoOutput>();
    output->setWindow(window);
    ANativeWindow_release(window);
    g_player->setVideoOutput(std::move(output));
  } else {
    g_player->setVideoOutput(nullptr);
  }
}

static void setupCallbacks() {
  if (!g_player || !g_callback)
    return;
  mediaplayer::PlaybackCallbacks cb;
  cb.onFinished = []() {
    if (!g_vm || !g_callback)
      return;
    JNIEnv *env = nullptr;
    if (g_vm->AttachCurrentThread(&env, nullptr) != JNI_OK)
      return;
    jclass cls = env->GetObjectClass(g_callback);
    jmethodID mid = env->GetMethodID(cls, "onFinished", "()V");
    env->CallVoidMethod(g_callback, mid);
    env->DeleteLocalRef(cls);
  };
  cb.onPosition = [](double pos) {
    if (!g_vm || !g_callback)
      return;
    JNIEnv *env = nullptr;
    if (g_vm->AttachCurrentThread(&env, nullptr) != JNI_OK)
      return;
    jclass cls = env->GetObjectClass(g_callback);
    jmethodID mid = env->GetMethodID(cls, "onPosition", "(D)V");
    env->CallVoidMethod(g_callback, mid, pos);
    env->DeleteLocalRef(cls);
  };
  g_player->setCallbacks(cb);
}
