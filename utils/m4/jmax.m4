dnl m4 macro to use with autoconf
dnl
dnl this macro find jmax-config and jmax.pc pkg-config file to get include directory, library directory and package directory
 
AC_DEFUN([AM_JMAX_CONFIG],[
dnl ##################################################
dnl jmax-config
dnl ##################################################


JMAX_FLAGS_SET="false"
AC_ARG_WITH(jmax-config,
	[ --with-jmax-config=PATH	path to jmax-config [optional]],
	[ JMAX_CONFIG="$withval";
	  dnl Set variable
	  JMAX_INCLUDES=`$JMAX_CONFIG --cflags`
   	  JMAX_LDFLAGS=`$JMAX_CONFIG --libs`
    	  JMAX_PACKAGE_DIR=`$JMAX_CONFIG --package-dir`				
   	  JMAX_FLAGS_SET=true	  
	],
	[
	  AC_MSG_WARN([*** Try to find jmax-config ***])
	]
	)

dnl ##################################################
dnl User flag for jmax-config
dnl ##################################################

if test $JMAX_FLAGS_SET = "false"; then dnl need automatic check for jmax-config

dnl ##################################################
dnl Automatic check for jmax-config 
dnl ##################################################
AC_PATH_PROG(JMAX_CONFIG, dnl variable name
	     jmax-config, dnl program to check
	     no           dnl value if not found
	     ) 

if test $JMAX_CONFIG = "no"; then
   AC_MSG_WARN([*** jmax-config not found, try to find jMax pkg-config configuration file *** ])
else
   JMAX_INCLUDES=`$JMAX_CONFIG --cflags`
   JMAX_LDFLAGS=`$JMAX_CONFIG --libs`
   JMAX_PACKAGE_DIR=`$JMAX_CONFIG --package-dir`				
   JMAX_FLAGS_SET=true
fi

fi dnl end of automatic check for jmax-config

if test $JMAX_FLAGS_SET = "false"; then dnl need automatic check for jmax pkg-config file
dnl ##################################################
dnl pkg-config
dnl ##################################################
dnl Check if pkg-config is availabe
AC_PATH_PROG(PKG_CONFIG, dnl variable name
             pkg-config, dnl program to check
	     no	         dnl value if not found
	     )

if test $PKG_CONFIG = "no"; then
   AC_MSG_WARN([*** pkg-config not found ***])
else
   dnl Check if jMax pkg-config configuration file is found 
   AC_MSG_CHECKING([jmax.pc (jMax pkg-config configuration file)])
   if $PKG_CONFIG --exists jmax ; then
      AC_MSG_RESULT(yes)
      JMAX_INCLUDES=`$PKG_CONFIG --cflags-only-I jmax`
      JMAX_LDFLAGS=`$PKG_CONFIG --libs jmax`
      JMAX_PACKAGE_DIR=`$PKG_CONFIG --variable=packagedir jmax`
      JMAX_FLAGS_SET=true
   fi
fi
fi dnl End of automatic check for pkg-config jmax file


dnl ##################################################
dnl result of automatic configuration
dnl ##################################################
dnl result of automatic jmax flags configuration 
if test $JMAX_FLAGS_SET = "false"; then
   AC_MSG_ERROR([jMax compilation flags are not set])
fi

dnl ##################################################
dnl overwrite package installation directory
dnl ##################################################
dnl if you want to install in another directory

dnl ##################################################
dnl Substitute variables in generated files
dnl ##################################################
AC_SUBST(JMAX_INCLUDES)	
AC_SUBST(JMAX_LDFLAGS)	
AC_SUBST(JMAX_PACKAGE_DIR)
])