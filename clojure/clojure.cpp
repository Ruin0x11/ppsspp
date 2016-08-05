#include "com_ruin_psp_PSP.h"
#include "../Core/Config.h"
#include "../Core/SaveState.h"
#include "../UI/EmuScreen.h"
#include "../Core/HLE/sceCtrl.h"
#include "../ext/native/base/Remote.h"
#include "../Core/MemMap.h"
#include "../Core/System.h"

#include <string.h>
#include <string>
#include <memory>

std::string gamePath;

int prevKeys = 0x0;

void Java_com_ruin_psp_PSP_greetSelf(JNIEnv *env, jclass clazz) {
  printf("dood, from PSP\n");
}

void Java_com_ruin_psp_PSP_startEmulator(JNIEnv *env, jclass clazz, jstring path) {
  const char *_imgName = env->GetStringUTFChars(path, 0);
  size_t len = strlen(_imgName);

  char imgName[len + 1];

  strcpy(imgName, _imgName);

  gamePath.assign(_imgName, len);

  char* arguments[] = {(char*)"PPSSPPDL", imgName};
  startEmu(2, arguments);

  step();
}

void Java_com_ruin_psp_PSP_step(JNIEnv *env, jclass clazz) {
  step();
}

void Java_com_ruin_psp_PSP_nstep(JNIEnv *env, jclass clazz, jint keys, jfloat analogX, jfloat analogY) {
  step(keys, prevKeys, analogX, analogY);
  prevKeys = keys;
}

void Java_com_ruin_psp_PSP_shutdown(JNIEnv *env, jclass clazz) {
  shutdown();
}

static void AfterSaveStateAction(bool success, const std::string &message, void *) {
  printf("%s\n", message.c_str());
}

void GetJStringContent(JNIEnv *AEnv, jstring AStr, std::string &ARes) {
  if (!AStr) {
    ARes.clear();
    return;
  }

  const char *s = AEnv->GetStringUTFChars(AStr,NULL);
  ARes=s;
  AEnv->ReleaseStringUTFChars(AStr,s);
}

void Java_com_ruin_psp_PSP_loadSaveState(JNIEnv *env, jclass clazz, jstring filename) {
  std::string str;
  GetJStringContent(env,filename,str);
  SaveState::Load(str, [](bool status, const std::string &message, void *) {
      if(!message.empty()) {
        puts(message.c_str());
      }
    });
}

void Java_com_ruin_psp_PSP_setFramelimit (JNIEnv *env, jclass clazz, jboolean framelimit) {
  if(framelimit) {
    PSP_CoreParameter().fpsLimit = 1;
  } else {
    PSP_CoreParameter().fpsLimit = 0;
  }
}

static bool isInInterval(u32 start, u32 end, u32 value)
{
  return start <= value && value < end;
}

static bool checkStart(u32 start, u32 size) {
  bool invalidSize = false;
  bool invalidAddress = false;

  if (isInInterval(PSP_GetScratchpadMemoryBase(),PSP_GetScratchpadMemoryEnd(),start))
    {
      invalidSize = !isInInterval(PSP_GetScratchpadMemoryBase(),PSP_GetScratchpadMemoryEnd(),start+size-1);
    } else if (isInInterval(PSP_GetVidMemBase(),PSP_GetVidMemEnd(),start))
    {
      invalidSize = !isInInterval(PSP_GetVidMemBase(),PSP_GetVidMemEnd(),start+size-1);
    } else if (isInInterval(PSP_GetKernelMemoryBase(),PSP_GetUserMemoryEnd(),start))
    {
      invalidSize = !isInInterval(PSP_GetKernelMemoryBase(),PSP_GetUserMemoryEnd(),start+size-1);
    } else
    {
      invalidAddress = true;
    }

  if (invalidAddress)
    {
      printf("Invalid address 0x%08X.\n",start);
      return false;
    } else if (invalidSize)
    {
      printf("Invalid end address 0x%08X.\n",start+size);
      return false;
    }
  return true;
}

jint Java_com_ruin_psp_PSP_readRAMU8(JNIEnv *env, jclass clazz, jint address) {
  u32 start = PSP_GetUserMemoryBase() + (u32)address;
  u32 size = 8;

  auto memLock = Memory::Lock();
  if (!PSP_IsInited())
    return 0;

  if (!checkStart(start, size))
    return 0;

  return (jint) Memory::Read_U8(start);
}

jint Java_com_ruin_psp_PSP_readRAMU16(JNIEnv *env, jclass clazz, jint address) {
  u32 start = PSP_GetUserMemoryBase() + (u32)address;
  u32 size = 16;

  auto memLock = Memory::Lock();
  if (!PSP_IsInited())
    return 0;

  if (!checkStart(start, size))
    return 0;

  return (jint) Memory::Read_U16(start);
}

jint Java_com_ruin_psp_PSP_readRAMU32(JNIEnv *env, jclass clazz, jint address) {
  u32 start = PSP_GetUserMemoryBase() + (u32)address;
  u32 size = 32;

  auto memLock = Memory::Lock();
  if (!PSP_IsInited())
    return 0;

  if (!checkStart(start, size))
    return 0;

  return (jint) Memory::Read_U32(start);
}

jfloat Java_com_ruin_psp_PSP_readRAMU32Float (JNIEnv *env, jclass clazz, jint address) {
  u32 start = PSP_GetUserMemoryBase() + (u32)address;
  u32 size = 32;

  auto memLock = Memory::Lock();
  if (!PSP_IsInited())
    return 0;

  if (!checkStart(start, size))
    return 0;

  u32 dat = Memory::Read_U32(start);
  float f;
  memcpy(&f, &dat, sizeof(f));

  return (jfloat) f;
}


jbyteArray Java_com_ruin_psp_PSP_readRam(JNIEnv *env, jclass clazz, jint address, jint size) {
  u32 start = PSP_GetUserMemoryBase() + (u32)address;

  auto memLock = Memory::Lock();
  if (!PSP_IsInited())
    return 0;

  if (!checkStart(start, size))
    return 0;

  u8 *ptr = Memory::GetPointer(start);

  jbyte arr[size];

  memcpy(&arr, ptr, sizeof(arr));

  jbyteArray ret = env->NewByteArray(size);
  env->SetByteArrayRegion(ret, 0, size, arr);
  return ret;
}
