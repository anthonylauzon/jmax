CXX = g++
SWIG = /usr/local/bin/swig

NAME = ftsclient

LIB = libftsclient.so
JMAX_INST_DIR = /var/tmp/JMAX
INC_CLIENT= -I$(JMAX_INST_DIR)/include
LIB_CLIENT= $(JMAX_INST_DIR)/lib/libftsclient.so

WRAP = $(NAME)_wrap.cpp
WRAP_OBJ = $(NAME)_wrap.o

INTERFACE = ../$(NAME).i

SWIG_OPT= -python -c++ -shadow
PYTHON_VERS=python2.2
PYTH_INCL=-I/usr/include/$(PYTHON_VERS)

MOD_NAME = _$(NAME).so

all: $(MOD_NAME)

$(WRAP): $(INTERFACE)
	$(SWIG) $(SWIG_OPT) $(INC_CLIENT) -o $@ $(INTERFACE) 

$(WRAP_OBJ): $(WRAP) $(INTERFACE)
	$(CXX) -c $(WRAP) $(PYTH_INCL) $(INC_CLIENT)

$(MOD_NAME): $(WRAP_OBJ)
	$(CXX) -shared $(WRAP_OBJ) -o $(MOD_NAME) $(LIB_CLIENT)

clean:
	rm -f $(MOD_NAME) $(OBJ) $(WRAP) $(WRAP_OBJ) *~
