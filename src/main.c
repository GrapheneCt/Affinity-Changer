#include <stdlib.h>
#include <string.h>
#include <vitasdk.h>
#include <psp2/io/fcntl.h>
#include <psp2/vshbridge.h> 
#include <psp2/power.h>
#include <psp2/shellutil.h> 

#include "debugScreen.h"
#include "ctrl.h"

#define printf psvDebugScreenPrintf

void flagger(char* path) {
	int fd;
	int affinityFlagAllUser = 0x08;
	fd = sceIoOpen(path, SCE_O_WRONLY, 0777);
	sceIoPwrite(fd, &affinityFlagAllUser, 0x01, 0x0294);
}

void deflagger(char* path) {
	int fd;
	int affinityFlagSystemCore = 0x00;
	fd = sceIoOpen(path, SCE_O_WRONLY, 0777);
	sceIoPwrite(fd, &affinityFlagSystemCore, 0x01, 0x0294);
}

void flag() {
	printf("Working on...\n");
	//Remount vs0: with RW permissions
	void *buf = malloc(0x100);
	vshIoUmount(0x300, 0, 0, 0);
	vshIoUmount(0x300, 1, 0, 0);
	_vshIoMount(0x300, 0, 2, buf);

	//Change CPU affinity mask to all user cores
	//PS Store: might cause lag in games that use modal PS Store
	//printf("PS Store\n");
	//flagger("vs0:app/NPXS10002/eboot.bin");
	//Friends
	printf("Friends\n");
	flagger("vs0:app/NPXS10006/eboot.bin");
	//Trophy
	printf("Trophies\n");
	flagger("vs0:app/NPXS10008/eboot.bin");
	//Music
	printf("Music\n");
	flagger("vs0:app/NPXS10009/eboot.bin");
	//Messages (Memory budget is 0x90 and uses phycont)
	printf("Messages\n");
	flagger("vs0:app/NPXS10014/eboot.bin");
	//Settings
	//printf("Settings\n");
	//flagger("vs0:app/NPXS10015/eboot.bin");
	//Checker (why not?)
	//printf("Checker\n");
	//flagger("vs0:app/NPXS10068/eboot.bin");
	//E-mail is already flagged
	//Calendar is already flagged
	//Kids
	printf("Parental Controls\n");
	flagger("vs0:app/NPXS10094/eboot.bin");
	printf("Done. Your system will restart in 5 seconds");
	sceKernelDelayThread(5000000);
	scePowerRequestColdReset();
}

void deflag() {
	printf("Working on...\n");
	//Remount vs0: with RW permissions
	void *buf = malloc(0x100);
	vshIoUmount(0x300, 0, 0, 0);
	vshIoUmount(0x300, 1, 0, 0);
	_vshIoMount(0x300, 0, 2, buf);

	//Change CPU affinity mask to system-reserved core
	//printf("PS Store\n");
	//PS Store
	//deflagger("vs0:app/NPXS10002/eboot.bin");
	//Friends
	printf("Friends\n");
	deflagger("vs0:app/NPXS10006/eboot.bin");
	//Trophy
	printf("Trophies\n");
	deflagger("vs0:app/NPXS10008/eboot.bin");
	//Music
	printf("Music\n");
	deflagger("vs0:app/NPXS10009/eboot.bin");
	//Messages
	printf("Messages\n");
	deflagger("vs0:app/NPXS10014/eboot.bin");
	//Settings
	//printf("Settings\n");
	//deflagger("vs0:app/NPXS10015/eboot.bin");
	//Checker
	//printf("Checker\n");
	//deflagger("vs0:app/NPXS10068/eboot.bin");
	//E-mail is already flagged
	//Calendar is already flagged
	//Kids
	printf("Parental Controls\n");
	deflagger("vs0:app/NPXS10094/eboot.bin");
	printf("Done. Your system will restart in 5 seconds");
	sceKernelDelayThread(5000000);
	scePowerRequestColdReset();
}

int main(){

	sceShellUtilInitEvents(0);
	
	psvDebugScreenInit();
	printf("\n\n--- Affinity Mask Switch for System Apps by Graphene ---\n");
	printf("CROSS: Set affinity mask to all user cores\n");
	printf("CIRCLE: Set affinity mask to system-reserved core\n\n");


	//Check for ref00d
	int rf;
	int ref = 0;
	if ((rf = sceIoOpen("ur0:tai/ref00d.skprx", SCE_O_RDONLY, 0777) < 0))
		goto checkFail;
	else 
		ref = 1;
	if ((rf = sceIoOpen("ux0:tai/ref00d.skprx", SCE_O_RDONLY, 0777) < 0 && ref == 0))
		goto checkFail;

	repeat:
	switch (get_key(0)) {
	case SCE_CTRL_CIRCLE:
		sceShellUtilLock(SCE_SHELL_UTIL_LOCK_TYPE_PS_BTN | SCE_SHELL_UTIL_LOCK_TYPE_QUICK_MENU);
		deflag();
		break;
	case SCE_CTRL_CROSS:
		sceShellUtilLock(SCE_SHELL_UTIL_LOCK_TYPE_PS_BTN | SCE_SHELL_UTIL_LOCK_TYPE_QUICK_MENU);
		flag();
		break;
	default:
		break;
	}
	sceKernelDelayThread(100000);
	goto repeat;

checkFail:

	printf("WARNING! reF00D.skprx not found. Do not attempt to use this application without it.\n");
	sceKernelDelayThread(5000000);

	return 0;
}
