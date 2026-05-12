#include <jni.h>
#include <string>
#include "mesh_engine.hpp"

static MadaMesh::MeshEngine g_engine;

extern "C" JNIEXPORT jstring JNICALL
Java_com_v0id_madamesh_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "MadaMesh Engine Online";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_v0id_madamesh_MainActivity_startEngine(
        JNIEnv* env,
        jobject /* this */,
        jint port) {
    return g_engine.start(port);
}

extern "C" JNIEXPORT void JNICALL
Java_com_v0id_madamesh_MainActivity_stopEngine(
        JNIEnv* env,
        jobject /* this */) {
    g_engine.stop();
}

extern "C" JNIEXPORT void JNICALL
Java_com_v0id_madamesh_MainActivity_discoverNodes(
        JNIEnv* env,
        jobject /* this */) {
    g_engine.discoverNodes();
}
