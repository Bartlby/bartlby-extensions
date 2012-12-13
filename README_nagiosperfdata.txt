this emulates nagios performance data 



gives follwing config variables

nagiosperfdata_format=DATATYPE::SERVICEPERFDATA TIMET::%d       HOSTNAME::$SERVER_ID$-$SERVER_NAME$     SERVICEDESC::$SERVICE_ID$-$SERVICE_NAME$        SERVICEPERFDATA::%s     SERVICECHECKCOMMAND::$SERVICE_PLUGIN$   HOSTSTATE::UP   HOSTSTATETYPE::HARD     SERVICESTATE::$READABLE_STATE$  SERVICESTATETYPE::HARD
nagiosperfdata_logfile=/opt/pnp4nagios/var/perfdata.log
