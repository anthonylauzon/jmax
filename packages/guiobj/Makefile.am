if COND_COMPILE_JAVA
JAVA_SRC_DIR= java
else
JAVA_SRC_DIR= 
endif

SUBDIRS=c $(JAVA_SRC_DIR) help

packagedir=$(pkgdatadir)/packages/guiobj
package_DATA=guiobj.jpkg

EXTRA_DIST= $(package_DATA)
