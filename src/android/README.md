# Android App

This directory contains the Android front end for the MediaPlayer project.
It is a standard Gradle project located in `app/`.

The app integrates the C++ core through JNI. Native sources are in `jni/` and
are built via the `CMakeLists.txt` included with the module. Kotlin code resides
under `app/src/main/java` and uses Jetpack components for navigation and
preferences.

Minimum SDK version is 24 and the project targets the latest API level.
Supported ABIs are `arm64-v8a`, `armeabi-v7a` and `x86_64`.

Run instrumented tests with `./gradlew connectedAndroidTest`.

The binary `gradle-wrapper.jar` is intentionally omitted from version control.
If `./gradlew` fails because the jar is missing, run the following command in
this directory to download it again:

```bash
gradle wrapper --gradle-version 8.1.1 --distribution-type bin
```

The command recreates `gradle/wrapper/gradle-wrapper.jar` so the wrapper script
can be used normally. CI environments can invoke the helper script
`devops/update_android_wrapper.sh` to automate this step.
