#ifndef _VM_H_
#define _VM_H_

#define FVM_RETURN           0
#define FVM_PUSH_INT         1
#define FVM_PUSH_FLOAT       2
#define FVM_PUSH_SYM         3
#define FVM_PUSH_BUILTIN_SYM 4
#define FVM_POP_ARGS         5
#define FVM_PUSH_OBJ         6
#define FVM_MV_OBJ           7
#define FVM_POP_OBJS         8
#define FVM_MAKE_OBJ         9
#define FVM_PUT_PROP        10
#define FVM_PUT_BUILTIN_PROP 11
#define FVM_OBJ_MESS        12
#define FVM_OBJ_BUILTIN_MESS        13
#define FVM_PUSH_OBJ_TABLE  14
#define FVM_POP_OBJ_TABLE   15
#define FVM_CONNECT         16

#endif
