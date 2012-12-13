this emulates nagios performance data 


there has to be one %d placeholder -> wich gets subsituted with the current unix timestamp 
and one %s - where the actual perfdata gets inserted

gives follwing config variables

nagiosperfdata_format=DATATYPE::SERVICEPERFDATA TIMET::%d       HOSTNAME::$SERVER_ID$-$SERVER_NAME$     SERVICEDESC::$SERVICE_ID$-$SERVICE_NAME$        SERVICEPERFDATA::%s     SERVICECHECKCOMMAND::$SERVICE_PLUGIN$   HOSTSTATE::UP   HOSTSTATETYPE::HARD     SERVICESTATE::$READABLE_STATE$  SERVICESTATETYPE::HARD
nagiosperfdata_logfile=/opt/pnp4nagios/var/perfdata.log
