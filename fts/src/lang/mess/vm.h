#ifndef _VM_H_
#define _VM_H_


/*
  Opcodes are built like this: 2 bits of argument size, and 6 of real opcode.
  The two bits are zero in the case where they are not needed.
  */

#define FVM_N_ARG               0x00
#define FVM_B_ARG               0x40
#define FVM_S_ARG               0x80
#define FVM_L_ARG               0xc0
  
#define FVM_RETURN             0

#define FVM_PUSH_INT           1
#define FVM_PUSH_INT_B         (FVM_PUSH_INT | FVM_B_ARG)
#define FVM_PUSH_INT_S         (FVM_PUSH_INT | FVM_S_ARG)
#define FVM_PUSH_INT_L         (FVM_PUSH_INT | FVM_L_ARG)

#define FVM_PUSH_FLOAT         2
#define FVM_PUSH_SYM           3

#define FVM_PUSH_SYM_B         (FVM_PUSH_SYM | FVM_B_ARG)
#define FVM_PUSH_SYM_S         (FVM_PUSH_SYM | FVM_S_ARG)
#define FVM_PUSH_SYM_L         (FVM_PUSH_SYM | FVM_L_ARG)

#define FVM_PUSH_BUILTIN_SYM   4

#define FVM_SET_INT            5
#define FVM_SET_INT_B          (FVM_SET_INT | FVM_B_ARG)
#define FVM_SET_INT_S          (FVM_SET_INT | FVM_S_ARG)
#define FVM_SET_INT_L          (FVM_SET_INT | FVM_L_ARG)

#define FVM_SET_FLOAT          6
#define FVM_SET_SYM            7

#define FVM_SET_SYM_B          (FVM_SET_SYM | FVM_B_ARG)
#define FVM_SET_SYM_S          (FVM_SET_SYM | FVM_S_ARG)
#define FVM_SET_SYM_L          (FVM_SET_SYM | FVM_L_ARG)

#define FVM_SET_BUILTIN_SYM    8

#define FVM_POP_ARGS           9
#define FVM_POP_ARGS_B         (FVM_POP_ARGS | FVM_B_ARG)
#define FVM_POP_ARGS_S         (FVM_POP_ARGS | FVM_S_ARG)
#define FVM_POP_ARGS_L         (FVM_POP_ARGS | FVM_L_ARG)


#define FVM_PUSH_OBJ          10
#define FVM_PUSH_OBJ_B        (FVM_PUSH_OBJ | FVM_B_ARG)
#define FVM_PUSH_OBJ_S        (FVM_PUSH_OBJ | FVM_S_ARG)
#define FVM_PUSH_OBJ_L        (FVM_PUSH_OBJ | FVM_L_ARG)

#define FVM_MV_OBJ            11
#define FVM_MV_OBJ_B          (FVM_MV_OBJ | FVM_B_ARG)
#define FVM_MV_OBJ_S          (FVM_MV_OBJ | FVM_S_ARG)
#define FVM_MV_OBJ_L          (FVM_MV_OBJ | FVM_L_ARG)

#define FVM_POP_OBJS          12
#define FVM_POP_OBJS_B        (FVM_POP_OBJS | FVM_B_ARG)
#define FVM_POP_OBJS_S        (FVM_POP_OBJS | FVM_S_ARG)
#define FVM_POP_OBJS_L        (FVM_POP_OBJS | FVM_L_ARG)


#define FVM_MAKE_OBJ          13
#define FVM_MAKE_OBJ_B        (FVM_MAKE_OBJ | FVM_B_ARG)
#define FVM_MAKE_OBJ_S        (FVM_MAKE_OBJ | FVM_S_ARG)
#define FVM_MAKE_OBJ_L        (FVM_MAKE_OBJ | FVM_L_ARG)


#define FVM_PUT_PROP          14
#define FVM_PUT_PROP_B        (FVM_PUT_PROP | FVM_B_ARG)
#define FVM_PUT_PROP_S        (FVM_PUT_PROP | FVM_S_ARG)
#define FVM_PUT_PROP_L        (FVM_PUT_PROP | FVM_L_ARG)


#define FVM_PUT_BUILTIN_PROP  15
#define FVM_OBJ_MESS          16
#define FVM_OBJ_BUILTIN_MESS  17


#define FVM_PUSH_OBJ_TABLE    18
#define FVM_PUSH_OBJ_TABLE_B  (FVM_PUSH_OBJ_TABLE | FVM_B_ARG)
#define FVM_PUSH_OBJ_TABLE_S  (FVM_PUSH_OBJ_TABLE | FVM_S_ARG)
#define FVM_PUSH_OBJ_TABLE_L  (FVM_PUSH_OBJ_TABLE | FVM_L_ARG)


#define FVM_POP_OBJ_TABLE     19
#define FVM_CONNECT           20

#define FVM_MAKE_TOP_OBJ      21
#define FVM_MAKE_TOP_OBJ_B    (FVM_MAKE_TOP_OBJ | FVM_B_ARG)
#define FVM_MAKE_TOP_OBJ_S    (FVM_MAKE_TOP_OBJ | FVM_S_ARG)
#define FVM_MAKE_TOP_OBJ_L    (FVM_MAKE_TOP_OBJ | FVM_L_ARG)


extern fts_object_t *fts_run_mess_vm(fts_object_t *parent,
				     unsigned char *program,
				     fts_symbol_t symbol_table[],
				     int ac, const fts_atom_t *at,
				     fts_expression_state_t *e);

extern void fts_vm_init();
#endif

