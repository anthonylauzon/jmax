if COND_COMPILE_JAVA
JAVA_SRC_DIR= java
else
JAVA_SRC_DIR= 
endif

SUBDIRS=c $(JAVA_SRC_DIR) help winbuild

packagedir=$(pkglibdir)/packages/guiobj
package_DATA=guiobj.jpkg

EXTRA_DIST= $(package_DATA)
