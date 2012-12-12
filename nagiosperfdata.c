#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <bartlby.h>

#define MY_CHECK_TYPE_ID 700

static struct shm_hdr * gHdr;
static void * gDataLoaderHandle;
static char * gCFG;


int nagiosperfdata_service_post_check(struct service * svc) {
	//_log("nagiosperfdata: POST check: %s/%s",svc->srv->server_name, svc->service_name);
	FILE * fp;
	int cur_time;
	char * the_perf_data;
	char * token;
	token=NULL;
	the_perf_data = strdup(svc->new_server_text);
	
	if(strstr(the_perf_data, "|") != NULL) {
	
		token = strtok(the_perf_data, "|");
		if(token != NULL) {
			token = strtok(NULL, "|");
			if(token != NULL) {
				fp = fopen("/opt/pnp4nagios/var/perfdata.log", "a");
				cur_time=time(NULL);
			
			
				if(fp != NULL) {
					fprintf(fp, "DATATYPE::SERVICEPERFDATA\tTIMET::%d\tHOSTNAME::%ld%s\tSERVICEDESC::%ld%s\tSERVICEPERFDATA::%s\tSERVICECHECKCOMMAND::%s\tHOSTSTATE::UP\tHOSTSTATETYPE::HARD\tSERVICESTATE::%s\tSERVICESTATETYPE::HARD\n",
					cur_time, svc->srv->server_id, svc->srv->server_name, svc->service_id, svc->service_name,token, svc->plugin, "OK");				
				
					fclose(fp);	
				}
			}
		}
	}
	free(the_perf_data);
	return EXTENSION_OK;
}

int bartlby_extension_dispatcher(int type, void * data) {
	int rtc;
	
	switch(type) {
		case EXTENSION_CALLBACK_POST_CHECK:
			rtc=nagiosperfdata_service_post_check((struct service *)data);
			return rtc;
			
			
		break;
		default:
			//_log("nagiosperfdata unkown message:%d", type);
			//Be gentle if we do not know what to do
			//do as all is OK to not block bartlby
			return EXTENSION_OK;
			
		
			
	}
	return EXTENSION_OK;
}


int bartlby_extension_startup(void * shm_addr, void * dataLoaderHandle, char * configfile) {
	_log("nagiosperfdata: %s", configfile);
	gHdr=shm_addr;
	gDataLoaderHandle=dataLoaderHandle;
	gCFG=configfile;
	return EXTENSION_OK;
}
int bartlby_extension_shutdown(int scheduler_end_code) {
	_log("nagiosperfdata: scheduler ended with %d", scheduler_end_code);
	
	return EXTENSION_OK;
}
