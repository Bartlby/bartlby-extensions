#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>
#include <mysql/mysql.h>
#include <openssl/sha.h>

#include <bartlby.h>

#define AUTOR "Helmut Januschka \"helmut@januschka.com\""
#define NAME "Logs Status Change History"
#define DLVERSION  "1.0"


struct service_hash_map {
	char sha1_hash[21];
	long service_id;	
	int last_write;
};


static struct service_hash_map  * state_hash_map;
static struct shm_header * gHdr;
static void * gDataLoaderHandle;
static char * gCFG;


static char * cfg_statehistory_dir;



char * GetName() {
	
	return strdup(NAME);
}

char * GetAutor() {
	
	return strdup(AUTOR);
}
char * GetVersion() {
	char * vers;
	asprintf(&vers, "%s", DLVERSION);
	return vers;
}

long ExpectVersion() {
	return EXPECTCORE;	
}	


int statehistory_find_shm_place_for_id(long service_id) {
	int x; 
	
	for(x=0; x<gHdr->svccount; x++) {
		if(state_hash_map[x].service_id == service_id) {
				return x;
		}
	}
	return -1;
	
}
void convertSHA1BinaryToCharStr(const unsigned char * const hashbin, char * const hashstr) {
  int i;
  
  
  for(i = 0; i<5; ++i)
  {
    sprintf(&hashstr[i*2], "%02x", hashbin[i]);
  }
  hashstr[10]=0;
}

int statehistory_check_will_run(struct service * svc) {
	unsigned char new_hash_bin[SHA_DIGEST_LENGTH];
	char new_hash_str[40];
	char * current_hash;
	char * file_path;
	int x;
	int diff;
	FILE * fp;
	
	time_t tnow;
	struct tm *tmnow;
	
	json_object * jso;

	char * log_path_copy;
	char * log_path_dirname;

	
	
	
	if(cfg_statehistory_dir == NULL) {
		return EXTENSION_OK;
	}
	
	SHA1(svc->current_output, strlen(svc->current_output), new_hash_bin);
	convertSHA1BinaryToCharStr(new_hash_bin, new_hash_str);
	x=statehistory_find_shm_place_for_id(svc->service_id);
	current_hash = state_hash_map[x].sha1_hash;
	
	if(strcmp(new_hash_str, current_hash) != 0 || diff >= 60*2) {
		
		diff=time(NULL)-state_hash_map[x].last_write;
		
		if(diff >= 30) { //FIXME: min time
			
			
			sprintf(state_hash_map[x].sha1_hash, new_hash_str);
			state_hash_map[x].last_write=time(NULL);
			
			time(&tnow);
			tmnow = localtime(&tnow);
			
			if (svc->current_output[strlen(svc->current_output) - 1] == '\n') {
  				svc->current_output[strlen(svc->current_output) - 1] == '\0';
			}
			
			asprintf(&file_path, "%s/%02d/%02d/%02d/%ld-%02d-%02d-%02d.history", cfg_statehistory_dir, tmnow->tm_year + 1900,tmnow->tm_mon + 1,tmnow->tm_mday, svc->service_id,tmnow->tm_year + 1900,tmnow->tm_mon + 1,tmnow->tm_mday);
			log_path_copy=strdup(file_path);
			log_path_dirname=dirname(log_path_copy);

			mkdir_recursive(log_path_dirname, 0777);

			free(log_path_copy);


			fp = fopen(file_path, "a");

			jso = json_object_new_object();
			json_object_object_add(jso,"current_state", json_object_new_int(svc->current_state));
			json_object_object_add(jso,"last_write", json_object_new_int(state_hash_map[x].last_write));
			json_object_object_add(jso,"output", json_object_new_string(svc->current_output));

			fprintf(fp,"%s\n#############REC##############\n", json_object_to_json_string(jso));

			json_object_put(jso);
			fclose(fp);
			free(file_path);
			
			
		}
	}
	
	
	
	
	
	
	//_log("statehistory: id->%ld text:%s", svc->service_id, svc->current_output);
	return EXTENSION_OK;
}

int bartlby_extension_dispatcher(int type, void * data) {
	int rtc;
	
	switch(type) {
		case EXTENSION_CALLBACK_CHECK_WILL_RUN:
//			
			rtc=statehistory_check_will_run((struct service *)data);
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
	int x;
	struct service * svcmap;
	unsigned char new_hash_bin[SHA_DIGEST_LENGTH];
	char new_hash_str[40];
	
	
	_log(LH_MOD, B_LOG_INFO, "statehistory: %s", configfile);
	
	cfg_statehistory_dir = getConfigValue("statehistory_logdir", configfile);
	
	
	gHdr=bartlby_SHM_GetHDR(shm_addr);
	gDataLoaderHandle=dataLoaderHandle;
	gCFG=configfile;
	svcmap=bartlby_SHM_ServiceMap(shm_addr);
	
	
	if(cfg_statehistory_dir == NULL) {
		_log(LH_MOD, B_LOG_CRIT, "statehistory you have to set 'statehistory_logdir'");
	}
	
	_log(LH_MOD, B_LOG_INFO, "statehistory: servicescount->%ld", gHdr->svccount);
	
	state_hash_map = malloc(sizeof(struct service_hash_map)*(gHdr->svccount+1));
	
	//initial hashing
	for(x=0; x<gHdr->svccount; x++) {
				state_hash_map[x].service_id=svcmap[x].service_id;
				SHA1(svcmap[x].current_output, strlen(svcmap[x].current_output), new_hash_bin);
				convertSHA1BinaryToCharStr(new_hash_bin, new_hash_str);				
				sprintf(state_hash_map[x].sha1_hash, "%s", new_hash_str);
				state_hash_map[x].last_write = time(NULL);
	}
	
	
	return EXTENSION_OK;
}
int bartlby_extension_shutdown(int scheduler_end_code) {
	_log(LH_MOD, B_LOG_INFO, "statehistory: scheduler ended with %d", scheduler_end_code);
	int x;
	free(state_hash_map);
	if(cfg_statehistory_dir != NULL) free(cfg_statehistory_dir);
	return EXTENSION_OK;
}
