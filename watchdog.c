#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>

#include <bartlby.h>



static struct shm_hdr * gHdr;
static void * gDataLoaderHandle;
static char * gCFG;


static int bartlby_watchdog_fd;




int bartlby_extension_dispatcher(int type, void * data) {
	int dummy;
	 //Just initiate a tick to reset HW timer			
	ioctl(bartlby_watchdog_fd, WDIOC_KEEPALIVE, &dummy);	
	
	return EXTENSION_OK;
}


int bartlby_extension_startup(void * shm_addr, void * dataLoaderHandle, char * configfile) {
	_log(LH_MOD, B_LOG_INFO, "watchdog: %s", configfile);
	gHdr=shm_addr;
	gDataLoaderHandle=dataLoaderHandle;
	gCFG=configfile;
	
	
	bartlby_watchdog_fd = open("/dev/watchdog",O_WRONLY);
	if(bartlby_watchdog_fd  == -1) {
		_log(LH_MOD, B_LOG_INFO, "watchdog: /dev/watchdog is not available..unloading module");
		return EXTENSION_NOK;
	}
	_log(LH_MOD, B_LOG_INFO, "watchdog: card initializing");
	ioctl(bartlby_watchdog_fd, WDIOC_SETOPTIONS, WDIOS_ENABLECARD);
	_log(LH_MOD, B_LOG_INFO, "watchdog: card initialized");
	return EXTENSION_OK;
	
	
	
	return EXTENSION_OK;
}
int bartlby_extension_shutdown(int scheduler_end_code) {
	ioctl(bartlby_watchdog_fd, WDIOC_SETOPTIONS, WDIOS_DISABLECARD);
	_log(LH_MOD, B_LOG_INFO, "watchdog: disabled");
	_log(LH_MOD, B_LOG_INFO, "watchdog: scheduler ended with %d", scheduler_end_code);
		
	return EXTENSION_OK;
}
