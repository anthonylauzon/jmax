dnl m4 macro to use with autoconf
dnl
dnl This file define AM_JMAX_CONFIG and AC_CHECK_JMAX_VERSION m4 macro.
dnl
dnl ########################################
dnl AM_JMAX_CONFIG m4 macro
dnl
dnl You can use this macro to get jMax external packages compilation and linking flag
dnl and for installation into jMax default packages path
dnl
dnl The AM_JMAX_CONFIG macro will substitute the following variables:
dnl
dnl JMAX_INCLUDES : cflags needed for jMax external packages compilation
dnl JMAX_LDFLAGS: ldflags needed for jMax external packages linking with fts
dnl JMAX_PACKAGE_DIR: default package path for jMax
dnl JMAX_VERSION: Version of jMax installed
dnl
 	
dnl ########################################
dnl AC_CHECK_JMAX_VERSION([MINIMAL-VERSION [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl
dnl The default version is 4.0.2

AC_DEFUN([AM_JMAX_CONFIG],[
dnl ##################################################
dnl jmax-config
dnl ##################################################
JMAX_FLAGS_SET="false"


dnl ##################################################
dnl User flag for jmax-config
dnl ##################################################
AC_ARG_WITH(jmax-config,
	[ --with-jmax-config=PATH	path to jmax-config [optional]],
	[ JMAX_CONFIG="$withval";
	  dnl Set variable
	  JMAX_INCLUDES=`$JMAX_CONFIG --cflags`
   	  JMAX_LDFLAGS=`$JMAX_CONFIG --libs`
    	  JMAX_PACKAGE_DIR=`$JMAX_CONFIG --package-dir`	
	  JMAX_VERSION=`$JMAX_CONFIG --version`			
   	  JMAX_FLAGS_SET=true	  
	],
	[
	  AC_MSG_WARN([*** Try to find jmax-config ***])
	]
	)

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
   JMAX_VERSION=`$JMAX_CONFIG --version`
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
      JMAX_VERSION=`$PKG_CONFIG --modversion jmax`
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
AC_SUBST(JMAX_VERSION)
])

dnl ########################################
dnl AC_CHECK_JMAX_VERSION
dnl 
dnl This macro check is jMax version is enough
dnl AC_CHECK_JMAX_VERSION([MINIMAL-VERSION [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl
dnl The default version is 4.0.2
dnl
AC_DEFUN(AC_CHECK_JMAX_VERSION,
[
dnl variable to store result of test
jmax_version_is_enough="no"

dnl Get the first argument if any, if no argumnet is given use 4.0.2 as default
min_jmax_version=ifelse([$1], ,4.0.2,$1)
AC_MSG_CHECKING(for jMax version >= $min_jmax_version)
    jmax_min_major_version=`echo $min_jmax_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    jmax_min_minor_version=`echo $min_jmax_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    jmax_min_micro_version=`echo $min_jmax_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
	
    dnl JMAX_VERSION is set by AM_JMAX_CONFIG
    dnl Don't take  for whatever we have
    dnl after the micro version
    jmax_installed_major_version=`echo $JMAX_VERSION | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)\(.*\)/\1/'`
    jmax_installed_minor_version=`echo $JMAX_VERSION | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)\(.*\)/\2/'`
    jmax_installed_micro_version=`echo $JMAX_VERSION | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)\(.*\)/\3/'`
    jmax_installed_additionnal_tag=`echo $JMAX_VERSION | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)\(.*\)/\4/'`

dnl Check for version
if test $jmax_installed_major_version -ge  $jmax_min_major_version ; then
	if test $jmax_installed_minor_version -ge  $jmax_min_minor_version ; then
		if test $jmax_installed_micro_version -ge  $jmax_min_micro_version ; then
			jmax_version_is_enough="yes"
		fi
	fi
fi

AC_MSG_RESULT($jmax_version_is_enough)
dnl Special check for unofficial release
if test $jmax_installed_additionnal_tag != "" ; then
	AC_MSG_WARN("your version of jMax is not an official release of jMax")
fi

dnl Do the specified action if any
if test "$jmax_version_is_enough" = "yes"; then
	ifelse([$2], , :, [$2])
else
	ifelse([$3], , :,[$3])
fi
])
