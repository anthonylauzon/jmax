dnl AC_CHECK_JAVA_VERSION check if java vesrion is good
dnl
dnl
dnl @author Patrice Tisserand <Patrice.Tisserand@ircam.fr>
AC_DEFUN([AC_CHECK_JAVA_VERSION],[
AC_REQUIRE([AC_PROG_JAVA])
ac_required_version=$1

AC_CACHE_CHECK([if $JAVA version is good], ac_cv_check_java_version, [

if AC_TRY_COMMAND($JAVA -version) > /dev/null 2>&1; then
  java_version=`$JAVA -version 2>&1 | grep "version" | sed 's/java version//' | sed 's/\"//g' `
  java_major_version=`echo $java_version | sed 's/\([[0-9]][[0-9]]*\)\..*/\1/'`
  java_minor_version=`echo $java_version | sed 's/[[0-9]][[0-9]]*\.\([[0-9]][[0-9]]*\)\(\.[[0-9]][[0-9]]*\)*/\1/' | sed 's/_.*//g'`

  required_major_version=`echo $ac_required_version  | sed 's/\([[0-9]][[0-9]]*\)\..*/\1/'`
  required_minor_version=`echo $ac_required_version | sed 's/[[0-9]][[0-9]]*\.\([[0-9]][[0-9]]*\)\(\.[[0-9]][[0-9]]*\)*/\1/' | sed 's/_.*//g'`

  if test "$required_major_version" -le "$java_major_version" -a "$required_minor_version" -le "$java_minor_version"; then
  ac_cv_check_java_version="yes";
  else
  ac_cv_check_java_version="no";
  fi
else
ac_cv_check_java_version="no";
fi
])
AC_PROVIDE([$0])
])
