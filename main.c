#include <psp2kern/kernel/modulemgr.h>
#include <string.h>
#include <taihen.h>

#define SCE_KERNEL_CPU_AFFINITY_FLAG_SYSTEM_CORE 8
#define SCE_KERNEL_CPU_AFFINITY_FLAG_ALL_USER    0

typedef SceUInt32 SceKernelProcessType;
typedef struct SceKernelProcessOpt {
	SceSize size;
	SceUInt32 unk_4;
	SceUInt32 cpuAffinityMask;
	SceUInt32 initPriority;
	SceSize stackSize;
	SceUInt32 unk_14;
	SceUInt32 budgetId;
	SceUInt32 unk_1C;
} SceKernelProcessOpt;

static tai_hook_ref_t hook_ref;
static SceUID hook_id;
static SceUID ksceKernelCreateProcess_patched(const char* name, SceKernelProcessType type, const char *path, SceKernelProcessOpt *pOpt)
{
	if (!strncmp(name, "NPXS", 4))
		//Take care of BG apps that may affect game performance
		if (strncmp(name, "NPXS10063", 9) != 0
			&& strncmp(name, "NPXS10083", 9) != 0
			&& strncmp(name, "NPXS10084", 9) != 0)
				pOpt->cpuAffinityMask = SCE_KERNEL_CPU_AFFINITY_FLAG_ALL_USER;

	return TAI_CONTINUE(SceUID, hook_ref, name, type, path, pOpt);
}

void _start() __attribute__ ((weak, alias("module_start")));
int module_start(SceSize argc, const void *args)
{
	hook_id = taiHookFunctionExportForKernel(
		KERNEL_PID,
		&hook_ref,
		"SceProcessmgr",
		0xEB1F8EF7,
		0x68068618,
		ksceKernelCreateProcess_patched);

	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
	if(hook_id >= 0) taiHookReleaseForKernel(hook_id, hook_ref);
	return SCE_KERNEL_STOP_SUCCESS;
}
