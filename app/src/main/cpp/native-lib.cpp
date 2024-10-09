#include <jni.h>
#include <string>
#include <thread>

// default example JNI interface (irrelevant to the repro).
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_native_1thread_1attach_MainActivity_attachThreadWithoutDetach(
        JNIEnv *env,
        jobject /* this */) {
    JavaVM *parent_vm = nullptr;
    env->GetJavaVM(&parent_vm);

    // create a single thread that attaches, but never detaches.
    auto t = std::thread([=] {
        JNIEnv *child_env = nullptr;
        parent_vm->GetEnv(reinterpret_cast<void **>(&child_env), JNI_VERSION_1_6);

        JavaVMAttachArgs args;
        args.version = JNI_VERSION_1_6;
        args.name = "non_detaching_thread";
        args.group = nullptr;

        // this adds the thread to the runtime thread-list, that
        // the tracer uses to sample all running runtime threads
        parent_vm->AttachCurrentThread(&child_env, &args);

#if IGNORED
        parent_vm->DetachCurrentThread();
#endif
    });

    // Join so that bionic removes this thread from the pthread thread-list. After join completion
    // the thread will still be in the runtime thread-list, leading tracer to sample a thread that
    // has no pthread equivalent. A `pthread_exit()` before Detach will achieve the same thing.
    t.join();

    // irrelevant return
    return env->NewStringUTF("joined before detaching");
}