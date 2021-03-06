#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/types.h>

#include <bartlby.h>

static void * gSHM;
static void * gDataLoaderHandle;
static char * gCFG;
static int lastUpdate;
static int updateIntervall;
static char * cfgStatusPath;
static int status_log_version;


void ext_nsc_clean_server_text(char * t) {
	int x;
	for(x=0; x<strlen(t); x++)  {
		if(t[x] == ';') {
			t[x]=' ';	
		}	
	}
		
}

void status_log_v1_out_start(FILE * fp, int cts, struct shm_header * h) {
	fprintf(fp,"[%d] PROGRAM;%d;2;1;0;0;1;1;1;1;1;1;1;1;\n", cts, h->startup_time);
	
}

void status_log_v2_out_start(FILE * fp, int cts) {
	fprintf(fp,"########################################\n");
	fprintf(fp,"#          NAGIOS STATUS FILE (by bartlby)\n");
	fprintf(fp,"#\n");
	fprintf(fp,"# THIS FILE IS AUTOMATICALLY GENERATED\n");
	fprintf(fp,"# BY NAGIOS.  DO NOT MODIFY THIS FILE!\n");
	fprintf(fp,"########################################\n\n");
	fprintf(fp,"info {\n");
	fprintf(fp,"\tcreated=%d\n",cts);
	fprintf(fp,"\tversion=%s-%s\n",PROGNAME, VERSION);
	fprintf(fp,"\t}\n\n");
}

void status_log_v2_output_service(FILE * fp, struct service * svc, int cts, int cs) {
	char * clean_server_text;

	
	clean_server_text=strdup(svc->new_server_text);
	ext_nsc_clean_server_text(clean_server_text);
	
	
	fprintf(fp,"service {\n");                                                                                                           
	fprintf(fp,"\thost_name=%s\n",svc->srv->server_name);                                                                              
	fprintf(fp,"\tservice_description=%s\n",svc->service_name);                                                                  
	fprintf(fp,"\tmodified_attributes=0\n");                                                         
	fprintf(fp,"\tcheck_command=%s\n",svc->plugin);               
	fprintf(fp,"\tevent_handler=\n");                               
	fprintf(fp,"\thas_been_checked=1\n");                                                                
	fprintf(fp,"\tshould_be_scheduled=1\n");                                                          
	fprintf(fp,"\tcheck_execution_time=0\n");                                                            
	fprintf(fp,"\tcheck_latency=0\n");                                                                          
	fprintf(fp,"\tcheck_type=%d\n",svc->service_type);                                                                            
	fprintf(fp,"\tcurrent_state=%d\n",svc->current_state);                                                                      
	fprintf(fp,"\tlast_hard_state=%d\n",svc->last_state);                                                                  
	fprintf(fp,"\tcurrent_attempt=%ld\n",svc->service_retain_current);                                                                  
	fprintf(fp,"\tmax_attempts=%ld\n",svc->service_retain);                                                                        
	fprintf(fp,"\tstate_type=1\n");                                                                            
	fprintf(fp,"\tlast_state_change=%d\n",cts);                                                             
	fprintf(fp,"\tlast_hard_state_change=d\n");                                                   
	fprintf(fp,"\tlast_time_ok=0\n"); 
	fprintf(fp,"\tlast_time_warning=0\n");                                                             
	fprintf(fp,"\tlast_time_unknown=0\n");                                                             
	fprintf(fp,"\tlast_time_critical=0\n");
	fprintf(fp,"\tplugin_output=%s\n",clean_server_text);                               
	fprintf(fp,"\tperformance_data=\n");                                    
	fprintf(fp,"\tlast_check=%d\n",svc->last_check);                                                                           
	fprintf(fp,"\tnext_check=%ld\n",svc->last_check + svc->check_interval);                                                                           
	fprintf(fp,"\tcurrent_notification_number=0\n");                                          
	fprintf(fp,"\tlast_notification=%d\n",svc->last_notify_send);                                                             
	fprintf(fp,"\tnext_notification=0\n");                                                             
	fprintf(fp,"\tno_more_notifications=0\n");                                                      
	fprintf(fp,"\tnotifications_enabled=%d\n",svc->notify_enabled);                                                      
	fprintf(fp,"\tactive_checks_enabled=1\n");                                                             
	fprintf(fp,"\tpassive_checks_enabled=0\n");                                             
	fprintf(fp,"\tevent_handler_enabled=0\n");                                                      
	fprintf(fp,"\tproblem_has_been_acknowledged=0\n");                                      
	fprintf(fp,"\tacknowledgement_type=0\n");                                                        
	fprintf(fp,"\tflap_detection_enabled=1\n");                                                    
	fprintf(fp,"\tfailure_prediction_enabled=0\n");                                            
	fprintf(fp,"\tprocess_performance_data=1\n");                                                
	fprintf(fp,"\tobsess_over_service=0\n");                                                          
	fprintf(fp,"\tlast_update=%d\n",cts);                                                                                      
	fprintf(fp,"\tis_flapping=0\n");                                                                          
	fprintf(fp,"\tpercent_state_change=0\n");                                                      
	fprintf(fp,"\tscheduled_downtime_depth=0\n");                                                                                   
	fprintf(fp,"\t}\n\n");  	
		
	free(clean_server_text);
}	

void status_log_v1_output_service(FILE * fp, struct service * svc, int cts, int cs) {
	char * hrState;
	char * clean_server_text;

	
	clean_server_text=strdup(svc->new_server_text);
	ext_nsc_clean_server_text(clean_server_text);
	
	hrState=bartlby_beauty_state(cs);
	//fprintf(fp, "[%d] SERVICE;%s;%s;%s;%ld/%ld;HARD;%d;%ld;PASSIVE;%d;0;0;%d;0;%s;0;0;0;0;%d;1;%d;0;100;0;0;10.0;0;0;0;0;%s\n", cts, svc->srv->server_name, svc->service_name, hrState, svc->service_retain_current, svc->service_retain, svc->last_check,svc->last_check+svc->check_interval,svc->service_active, svc->notify_last_time, hrState, svc->notify_last_time, svc->notify_enabled, clean_server_text);
	free(hrState);
	free(clean_server_text);	
}
void updateNSCFile() {
	struct service	* svcmap;
	struct shm_header * hdr;
	FILE * statusFP;
	
	char * tmpFP;
	
	int cts;
	int x;
	int cs;
	
	if(cfgStatusPath == NULL) {
		_log(LH_MOD, B_LOG_CRIT, "'extensions.nagios.status.log.path' unset ????");
		return;	
	}
	
	hdr = bartlby_SHM_GetHDR(gSHM);
	svcmap = bartlby_SHM_ServiceMap(gSHM);
	
	tmpFP=malloc(sizeof(char)*strlen(cfgStatusPath)+20);
	sprintf(tmpFP, "%s.tmp", cfgStatusPath);
	
	statusFP = fopen(tmpFP, "w");
	if(!statusFP) {
		_log(LH_MOD, B_LOG_CRIT, "fopen() '%s' failed", cfgStatusPath);
		return;	
	}
	cts=time(NULL);
	if(status_log_version == 2) {
		status_log_v2_out_start(statusFP, cts);	
	} else {
		status_log_v1_out_start(statusFP, cts, hdr);	
	}
	for(x=0; x<hdr->svccount; x++) {
		//_log("ext: nsc: %s:%d/%s", svcmap[x].server_name, svcmap[x].client_port, svcmap[x].service_name);
		
		if(svcmap[x].current_state == 0 ||svcmap[x].current_state == 1|| svcmap[x].current_state == 2) {
			cs = svcmap[x].current_state;		
		} else {
			cs = 1; //default to warning for NSC
		}
		
		
		if(status_log_version == 2) {
			status_log_v2_output_service(statusFP, &svcmap[x], cts, cs);	
		} else {
			status_log_v1_output_service(statusFP, &svcmap[x], cts, cs);
			
		}
		
		
	}
	fclose(statusFP);
	
	rename(tmpFP, cfgStatusPath);
	free(tmpFP);
	
	
}



int nsc_round_time(struct shm_header * hdr) {
	int diff;
	diff=time(NULL)-lastUpdate;	
	if(diff >= updateIntervall) {
		updateNSCFile();
		lastUpdate=time(NULL);
	}
	return EXTENSION_OK;	
}

int bartlby_extension_dispatcher(int type, void * data) {
	int rtc;
	
	switch(type) {
		
		case EXTENSION_CALLBACK_ROUND_TIME:
			rtc=nsc_round_time((struct shm_header *) data);
			return rtc;
		break;
					
	}
	return EXTENSION_OK;
}


int bartlby_extension_startup(void * shm_address, void * dataLoaderHandle, char * configfile) {
	char * cfgInterval;
	char * cfgLogVersion;
	set_cfg(configfile);
	_log(LH_MOD, B_LOG_INFO, "Nagios status.log file support for bartlby instance @ %s", configfile);
	gSHM=shm_address;
	gDataLoaderHandle=dataLoaderHandle;
	gCFG=configfile;
	lastUpdate = time(NULL);
	
	
	cfgInterval = getConfigValue("extensions.nagios.status.log.updatefrequence", configfile);
	cfgStatusPath = getConfigValue("extensions.nagios.status.log.path", configfile);
	cfgLogVersion = getConfigValue("extensions.nagios.status.log.version", configfile);
	if(cfgLogVersion) {
		status_log_version = atoi(cfgLogVersion);
		free(cfgLogVersion);
	} else {
		_log(LH_MOD, B_LOG_INFO, "ext: nsc: 'extensions.nagios.status.log.version' not set defaulting to version 2 format");
		status_log_version = 2;	
	}
	if(cfgInterval) {
		updateIntervall = atoi(cfgInterval);	
		free(cfgInterval);
	} else {
		_log(LH_MOD, B_LOG_INFO, "ext: nsc 'extensions.nagios.status.log.updatefrequence' not set in config defaulting to 10)");
		updateIntervall = 10;
	}
	
	
	return EXTENSION_OK;
}
int bartlby_extension_shutdown(int scheduler_end_code) {
	_log(LH_MOD, B_LOG_INFO, "nagios nsc: scheduler ended with %d", scheduler_end_code);
	if(cfgStatusPath != NULL) {
		free(cfgStatusPath);	
	}
	return EXTENSION_OK;
}
