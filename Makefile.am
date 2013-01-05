AUTOMAKE_OPTIONS = subdir-objects foreign
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = -I$(top_srcdir)/../bartlby-core/include
AM_LDFLAGS =  -avoid-version -module
lib_LTLIBRARIES = nagiosperfdata.la extlogger.la distributive.la  helloworld.la nsc.la watchdog.la

nagiosperfdata_la_SOURCES = ../bartlby-core/src/global.c ../bartlby-core/src/config.c nagiosperfdata.c
extlogger_la_SOURCES = ../bartlby-core/src/global.c ../bartlby-core/src/config.c extlogger.c
distributive_la_SOURCES = ../bartlby-core/src/global.c ../bartlby-core/src/config.c distributive.c
helloworld_la_SOURCES = ../bartlby-core/src/global.c ../bartlby-core/src/config.c helloworld.c
nsc_la_SOURCES = ../bartlby-core/src/global.c ../bartlby-core/src/config.c nsc.c
watchdog_la_SOURCES = ../bartlby-core/src/global.c ../bartlby-core/src/config.c watchdog.c









dist_noinst_SCRIPTS = autogen.sh