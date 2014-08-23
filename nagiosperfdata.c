#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <bartlby.h>

#define AUTOR "Helmut Januschka \"helmut@januschka.com\" http://bartlby.org"
#define NAME "Nagios Perf Data supports pnp4nagios"
#define DLVERSION  "1.0"


static struct shm_hdr * gHdr;
static void * gDataLoaderHandle;
static char * gCFG;


static char * cfg_perflog_file;
static char * cfg_output_format;

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



int nagiosperfdata_service_post_check(struct service * svc) {
	//_log("nagiosperfdata: POST check: %s/%s",svc->srv->server_name, svc->service_name);
	FILE * fp;
	int cur_time;
	char * the_perf_data;
	char * token, * token1;
	int perflog_mem;
	char * perflog_msg;
	
	int token_len;
	token=NULL;
	
	
	if(cfg_perflog_file == NULL || cfg_perflog_file == NULL) {
			return EXTENSION_OK;
	}
	
	
	the_perf_data = strdup(svc->new_server_text);
	
	
	if(strstr(the_perf_data, "|") != NULL) {
	
		token = strtok(the_perf_data, "|");
		
		if(token != NULL) {
			token = strtok(NULL, "|");
			if(token != NULL) {
				if(strstr(token, "\n") != NULL) {
					//MULTILINE
					token1=strtok(token, "\n");
					token=token1;
									
				}
				fp = fopen(cfg_perflog_file, "a");
				cur_time=time(NULL);
			
			
				if(fp != NULL) {
					
					perflog_mem=(strlen(cfg_output_format)+40+strlen(svc->service_name)+strlen(PROGNAME)+strlen(VERSION)+strlen(svc->srv->server_name)+strlen(svc->service_name)+40+strlen(svc->new_server_text));
					perflog_msg=malloc(sizeof(char)*perflog_mem);
					sprintf(perflog_msg, "%s\n", cfg_output_format);
	
					token_len = strlen(token);
					if( token[token_len-1] == '\n' ) 	token[token_len-1] = 0;
						
					bartlby_replace_svc_in_str(perflog_msg, svc, perflog_mem);
		
					fprintf(fp, perflog_msg ,	cur_time,token);				
					fclose(fp);	
					
					free(perflog_msg);
					
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
	_log(LH_MOD, B_LOG_INFO, "nagiosperfdata: %s", configfile);
	gHdr=shm_addr;
	gDataLoaderHandle=dataLoaderHandle;
	gCFG=configfile;
	
	cfg_perflog_file = getConfigValue("nagiosperfdata_logfile", configfile);
	cfg_output_format = getConfigValue("nagiosperfdata_format", configfile);
	                    
	if(cfg_perflog_file == NULL || cfg_perflog_file == NULL) {
			_log(LH_MOD, B_LOG_CRIT, "nagiosperfdata you have to set 'nagiosperfdata_logfile' and 'nagiosperfdata_format'");
	}	                    
	                     
	return EXTENSION_OK;
}
int bartlby_extension_shutdown(int scheduler_end_code) {
	_log(LH_MOD, B_LOG_INFO, "nagiosperfdata: scheduler ended with %d", scheduler_end_code);

	if(cfg_perflog_file != NULL) free(cfg_perflog_file);
	if(cfg_output_format != NULL) free(cfg_output_format);
	
	return EXTENSION_OK;
}
