#include "android/AndroidGLVideoOutput.h"
#include "mediaplayer/MediaPlayer.h"
#include <android/native_window_jni.h>
#include <jni.h>
#include <memory>
#include <string>

using mediaplayer::MediaPlayer;

static std::unique_ptr<MediaPlayer> g_player;
static jobject g_callback = nullptr;
static jmethodID g_onFinished = nullptr;
static jmethodID g_onPosition = nullptr;
static JavaVM *g_vm = nullptr;

static JNIEnv *getEnv(bool *detach) {
  JNIEnv *env = nullptr;
  if (g_vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) == JNI_EDETACHED) {
    if (g_vm->AttachCurrentThread(&env, nullptr) != 0)
      return nullptr;
    if (detach)
      *detach = true;
  }
  return env;
}

extern "C" jint JNI_OnLoad(JavaVM *vm, void *) {
  g_vm = vm;
  return JNI_VERSION_1_6;
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
  if (!path)
    return JNI_FALSE;

  const char *cpath = env->GetStringUTFChars(path, nullptr);
  if (!cpath)
    return JNI_FALSE;
  std::string cppPath(cpath);
  env->ReleaseStringUTFChars(path, cpath);

  if (!g_player)
    g_player = std::make_unique<MediaPlayer>();
  bool ok = g_player->open(cppPath);
  return ok ? JNI_TRUE : JNI_FALSE;
}

extern "C" void
Java_com_example_mediaplayer_MediaPlayerNative_nativeSetCallback(JNIEnv *env, jclass, jobject cb) {
  if (g_callback) {
    env->DeleteGlobalRef(g_callback);
    g_callback = nullptr;
  }
  g_onFinished = nullptr;
  g_onPosition = nullptr;

  if (cb) {
    g_callback = env->NewGlobalRef(cb);
    jclass cls = env->GetObjectClass(cb);
    g_onFinished = env->GetMethodID(cls, "onPlaybackFinished", "()V");
    g_onPosition = env->GetMethodID(cls, "onPositionChanged", "(D)V");
    env->DeleteLocalRef(cls);
  }

  if (!g_player)
    g_player = std::make_unique<MediaPlayer>();

  mediaplayer::PlaybackCallbacks cbs;
  if (g_callback) {
    cbs.onFinished = []() {
      bool detach = false;
      JNIEnv *envCb = getEnv(&detach);
      if (envCb && g_callback && g_onFinished)
        envCb->CallVoidMethod(g_callback, g_onFinished);
      if (detach)
        g_vm->DetachCurrentThread();
    };
    cbs.onPosition = [](double pos) {
      bool detach = false;
      JNIEnv *envCb = getEnv(&detach);
      if (envCb && g_callback && g_onPosition)
        envCb->CallVoidMethod(g_callback, g_onPosition, pos);
      if (detach)
        g_vm->DetachCurrentThread();
    };
  }
  g_player->setCallbacks(cbs);
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

extern "C" void Java_com_example_mediaplayer_MediaPlayerNative_nativeNextTrack(JNIEnv *, jclass) {
  if (g_player)
    g_player->nextTrack();
}

extern "C" void Java_com_example_mediaplayer_MediaPlayerNative_nativePreviousTrack(JNIEnv *,
                                                                                   jclass) {
  if (g_player)
    g_player->previousTrack();
}

extern "C" void
Java_com_example_mediaplayer_MediaPlayerNative_nativeEnableShuffle(JNIEnv *, jclass,
                                                                   jboolean enabled) {
  if (g_player)
    g_player->enableShuffle(enabled == JNI_TRUE);
}

extern "C" void Java_com_example_mediaplayer_MediaPlayerNative_nativeSetSurface(JNIEnv *env, jclass,
                                                                                jobject surface) {
  if (!g_player)
    g_player = std::make_unique<MediaPlayer>();

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
