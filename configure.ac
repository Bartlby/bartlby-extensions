AC_INIT([bartlby-extensions], [1.4.0], [helmut@januschka.com],1.6.0, [http://bartlby.org/])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 no-define])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])
AM_SILENT_RULES([yes])
AC_CONFIG_FILES([Makefile])
AC_CHECK_FILE(
   [../bartlby-core/libbartlby.a],
   [AC_MSG_NOTICE([Found libbartlby.a])],
   [AC_MSG_ERROR([Cannot find libbartlby.a please checkout bartlby-core in the ../bartlby-core/ folder and compile it so you get a libbartlby.a lib])]
   )
AC_OUTPUT
