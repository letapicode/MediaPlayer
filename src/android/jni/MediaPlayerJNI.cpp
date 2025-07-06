#include "android/AndroidGLVideoOutput.h"
#include "mediaplayer/MediaPlayer.h"
#include <android/native_window_jni.h>
#include <jni.h>
#include <memory>
#include <string>

using mediaplayer::MediaPlayer;

static std::unique_ptr<MediaPlayer> g_player;
static jobject g_callback = nullptr;
static JavaVM *g_vm = nullptr;

static JNIEnv *getEnv() {
  JNIEnv *env = nullptr;
  if (g_vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
    g_vm->AttachCurrentThread(&env, nullptr);
  }
  return env;
}

static void callVoid(const char *name) {
  if (!g_callback)
    return;
  JNIEnv *env = getEnv();
  jclass cls = env->GetObjectClass(g_callback);
  jmethodID mid = env->GetMethodID(cls, name, "()V");
  if (mid)
    env->CallVoidMethod(g_callback, mid);
  env->DeleteLocalRef(cls);
}

static void callDouble(const char *name, double value) {
  if (!g_callback)
    return;
  JNIEnv *env = getEnv();
  jclass cls = env->GetObjectClass(g_callback);
  jmethodID mid = env->GetMethodID(cls, name, "(D)V");
  if (mid)
    env->CallVoidMethod(g_callback, mid, value);
  env->DeleteLocalRef(cls);
}

static void callString(const char *name, const std::string &str) {
  if (!g_callback)
    return;
  JNIEnv *env = getEnv();
  jclass cls = env->GetObjectClass(g_callback);
  jmethodID mid = env->GetMethodID(cls, name, "(Ljava/lang/String;)V");
  if (mid) {
    jstring jstr = env->NewStringUTF(str.c_str());
    env->CallVoidMethod(g_callback, mid, jstr);
    env->DeleteLocalRef(jstr);
  }
  env->DeleteLocalRef(cls);
}

static void updateCallbacks() {
  if (!g_player)
    return;
  mediaplayer::PlaybackCallbacks cb{};
  cb.onPlay = []() { callVoid("onPlay"); };
  cb.onPause = []() { callVoid("onPause"); };
  cb.onStop = []() { callVoid("onStop"); };
  cb.onFinished = []() { callVoid("onFinished"); };
  cb.onTrackLoaded = [](const mediaplayer::MediaMetadata &m) {
    callString("onTrackLoaded", m.path);
  };
  cb.onPosition = [](double p) { callDouble("onPosition", p); };
  cb.onError = [](const std::string &msg) { callString("onError", msg); };
  g_player->setCallbacks(std::move(cb));
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
  if (!g_player)
    g_player = std::make_unique<MediaPlayer>();
  const char *cpath = env->GetStringUTFChars(path, nullptr);
  updateCallbacks();
  bool ok = g_player->open(cpath);
  env->ReleaseStringUTFChars(path, cpath);
  return ok ? JNI_TRUE : JNI_FALSE;
}

extern "C" void
Java_com_example_mediaplayer_MediaPlayerNative_nativeSetListener(JNIEnv *env, jclass, jobject cb) {
  if (g_callback)
    env->DeleteGlobalRef(g_callback);
  g_callback = cb ? env->NewGlobalRef(cb) : nullptr;
  updateCallbacks();
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
