if COND_COMPILE_JAVA
JAVA_SRC_DIR= java
else
JAVA_SRC_DIR=
endif

SUBDIRS=c $(JAVA_SRC_DIR) help images winbuild

packagedir=$(pkglibdir)/packages/data
package_DATA=data.jpkg

EXTRA_DIST= $(package_DATA)
