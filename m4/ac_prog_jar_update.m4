dnl AC_PROG_JAR_UPDATE tests if "-uf" option is available with current jar
dnl
dnl
dnl @author Patrice Tisserand <Patrice.Tisserand@ircam.fr>
AC_DEFUN([AC_PROG_JAR_UPDATE],[
AC_CACHE_CHECK([if $JAR support -u flag], ac_cv_prog_jar_update, [
dnl Output variable 
JAR_SUPPORT_UPDATE="no"

dnl Setup for temporary variable
JAR_TEST_TMP_ROOT=/tmp
FILE_TO_PUT_IN_JAR=$JAR_TEST_TMP_ROOT/my_file.$$
JAR_FILE_TO_UPDATE=$JAR_TEST_TMP_ROOT/my_jar.$$

dnl Touch file for update
touch $FILE_TO_PUT_IN_JAR
touch $JAR_FILE_TO_UPDATE

dnl Check result of $JAR -uf
if AC_TRY_COMMAND($JAR -uf $JAR_FILE_TO_UPDATE $FILE_TO_PUT_IN_JAR) > /dev/null 2>&1; then
    ac_cv_prog_jar_update="yes"
else
    ac_cv_prog_jar_update="no"
fi

dnl Remove temporary files
rm $JAR_FILE_TO_UPDATE
rm $FILE_TO_PUT_IN_JAR
])
AC_PROVIDE([$0]) dnl
])
