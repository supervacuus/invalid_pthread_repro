## Repro for invalid pthread_t 0x... passed to pthread_getcpuclockid

This is a repro of the abort seen in the following issue: https://github.com/getsentry/sentry-java/issues/2604.
It is based on a basic Android Studio app template. The only things that changed were the following:

* text content and size of the centered `TextView`, which is clickable to trigger the repro.
* the `MainActivity` starts the Android method tracer `onCreate` in background sampling mode (using the same parameters as the Android SDK).
* an `OnClickListener` calling the native code that triggers the abort.

### What happens in the native code?

* The JNI function starts a native thread (appending it to the pthread thread list)
* the native thread attaches to the ART (appending it to the ART thread list)
* but exits before detaching (-> the thread remains in the ART thread list)
* the JNI function joins that thread ( removing it from the pthread thread list)

  https://github.com/supervacuus/invalid_pthread_repro/blob/0a1679fd3b14b1229376865ebe82f54e76af3fbc/app/src/main/cpp/native-lib.cpp#L10-L35

### Why does that lead to an abort?

* Calling `pthread_exit()` or `pthread_join()` before an attached thread could be detached is illegal.
* But that doesn't necessarily mean that it will fail at that point.
* Since the two thread lists are now out of sync, the ART method tracer will retrieve a thread from the ART thread list that is no longer maintained in `pthread` state
* The first function requiring a look-up in the tracer sampling path seems to be `pthread_getcpuclockid()` [1], which aborts with a fatal log if the provided pthread_t cannot be found [2].


[1] https://cs.android.com/android/platform/superproject/main/+/main:bionic/libc/bionic/pthread_getcpuclockid.cpp;l=36

[2] https://cs.android.com/android/platform/superproject/main/+/main:bionic/libc/bionic/pthread_internal.cpp;l=119-130
