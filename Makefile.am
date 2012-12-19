AUTOMAKE_OPTIONS = subdir-objects foreign
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = -I$(top_srcdir)/../bartlby-core/include

lib_LTLIBRARIES = libnagiosperfdata.la libextlogger.la libdistributive.la libhelloworld.la libnsc.la libwatchdog.la

libnagiosperfdata_la_SOURCES = ../bartlby-core/src/global.c ../bartlby-core/src/config.c nagiosperfdata.c
libextlogger_la_SOURCES = ../bartlby-core/src/global.c ../bartlby-core/src/config.c extlogger.c
libdistributive_la_SOURCES = ../bartlby-core/src/global.c ../bartlby-core/src/config.c distributive.c
libhelloworld_la_SOURCES = ../bartlby-core/src/global.c ../bartlby-core/src/config.c helloworld.c
libnsc_la_SOURCES = ../bartlby-core/src/global.c ../bartlby-core/src/config.c nsc.c
libwatchdog_la_SOURCES = ../bartlby-core/src/global.c ../bartlby-core/src/config.c watchdog.c









dist_noinst_SCRIPTS = autogen.sh