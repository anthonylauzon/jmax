# # C++ link problems on Mac OS X
# #SUBDIRS= cpp include java
# SUBDIRS= include java
# Since Mac OS X doesn't use anymore automake we restore previous variable
# value
if COND_COMPILE_JAVA
JAVA_SRC_DIR= java
else
JAVA_SRC_DIR= 
endif

PYTHON_SRC_DIR = python

SUBDIRS= $(JAVA_SRC_DIR) cpp $(PYTHON_SRC_DIR) winbuild



