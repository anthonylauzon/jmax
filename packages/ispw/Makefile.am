if COND_COMPILE_JAVA
JAVA_SRC_DIR= java
else
JAVA_SRC_DIR= 
endif

SUBDIRS=c $(JAVA_SRC_DIR) help images sounds

packagedir=$(pkgdatadir)/packages/ispw
package_DATA=ispw.jpkg

EXTRA_DIST= $(package_DATA)
