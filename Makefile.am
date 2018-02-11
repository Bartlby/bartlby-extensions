AUTOMAKE_OPTIONS = subdir-objects foreign
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = -I$(top_srcdir)/../bartlby-core/src/build/include/ -I$(top_srcdir)/../bartlby-core/src/deps/ -I$(top_srcdir)/../bartlby-core/include -I/usr/include/json-c -I$(top_srcdir)/../bartlby-core/src/buffer/
AM_LDFLAGS =  -avoid-version -module -lssl  -avoid-version -module -Wl,--whole-archive,../bartlby-core/build/libbartlby.a,--no-whole-archive -g


lib_LTLIBRARIES = nagiosperfdata.la extlogger.la distributive.la  helloworld.la nsc.la watchdog.la statehistory.la

nagiosperfdata_la_SOURCES =  nagiosperfdata.c
extlogger_la_SOURCES =  extlogger.c
distributive_la_SOURCES =  distributive.c
helloworld_la_SOURCES =  helloworld.c
nsc_la_SOURCES =  nsc.c
watchdog_la_SOURCES =  watchdog.c
statehistory_la_SOURCES =  statehistory.c











dist_noinst_SCRIPTS = autogen.sh
