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

void Java_com_ruin_psp_PSP_greetSelf(JNIEnv *env, jclass clazz) {
  printf("dood, from PSP\n");
}

void Java_com_ruin_psp_PSP_startEmulator(JNIEnv *env, jclass clazz, jstring path) {
  const char *_imgName = env->GetStringUTFChars(path, 0);
  size_t len = strlen(_imgName);

  char imgName[len + 1];

  strcpy(imgName, _imgName);

  gamePath.assign(_imgName, len);

  char* arguments[] = {"PPSSPPDL", imgName};
  startEmu(2, arguments);

  step();
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

static void AfterSaveStateAction(bool success, const std::string &message, void *) {
  printf("%s\n", message.c_str());
}

void Java_com_ruin_psp_PSP_loadSaveState(JNIEnv *env, jclass clazz, jint slot) {
  SaveState::LoadSlot(gamePath, slot, &AfterSaveStateAction);
  g_Config.iCurrentStateSlot = slot;
}

static bool isInInterval(u32 start, u32 end, u32 value)
{
	return start <= value && value < end;
}

jint Java_com_ruin_psp_PSP_readRAM(JNIEnv *env, jclass clazz, jint address) {
	u32 start = PSP_GetUserMemoryBase() + (u32)address;
        u32 size = 16;
        printf("0x%08X\n", start);

	auto memLock = Memory::Lock();
	if (!PSP_IsInited())
		return 0;

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
		return 0;
	} else if (invalidSize)
	{
		printf("Invalid end address 0x%08X.\n",start+size);
		return 0;
	}

	return (jint) *Memory::GetPointer(start);
}
