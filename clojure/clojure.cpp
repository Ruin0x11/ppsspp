#include "com_ruin_psp_PSP.h"
#include "../Core/Config.h"
#include "../ext/native/base/Remote.h"

#include <string.h>

void Java_com_ruin_psp_PSP_greetSelf(JNIEnv *env, jclass clazz) {
  printf("dood, from PSP\n");
}

void Java_com_ruin_psp_PSP_startEmulator(JNIEnv *env, jclass clazz, jstring path) {
  const char *_imgName = env->GetStringUTFChars(path, 0);
  size_t len = strlen(_imgName);
  
  char imgName[len + 1];

  strcpy(imgName, _imgName);
 
  char* arguments[] = {"PPSSPPDL", imgName};
  startEmu(1, arguments);
}

void Java_com_ruin_psp_PSP_step(JNIEnv *env, jclass clazz) {
  step();
}

void Java_com_ruin_psp_PSP_nstep(JNIEnv *env, jclass clazz, jint key) {
  step(key);
}

void Java_com_ruin_psp_PSP_shutdown(JNIEnv *env, jclass clazz) {
  shutdown();
}
