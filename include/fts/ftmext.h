/*
 * ftmext.h allows to write externals that compile under 
 * jMax and Max/MSP with FTM
 */


#ifndef _JMAX_FTMEXT_H_
#define _JMAX_FTMEXT_H_


#define FTM_JMAX


/*
 * class instantiation
 */

/* class init var 1 */
#if 0

#define FTM_INSTANTIATE(class)  \
    void class##_instantiate(fts_class_t *cl)

#define ftm_class_init(class)	\
    fts_class_init(FTM_GET_CLASS, sizeof(class##_t), class##_init, class##_delete);

#define ftm_get_class()		(cl)

#endif


/* class init var 2 */
#define FTM_INSTANTIATE(func)  \
void func(fts_class_t *cl)

#define ftm_init(size, initfunc, deletefunc)	\
fts_class_init(cl, size, initfunc, deletefunc)


/*
 * package config
 */

#define FTM_CONFIG(module) \
void module##_config (void)

#define ftm_install(sym, func) \
fts_class_install(sym, func)



/*
 * messages
 */

/* method function header */
#define FTM_MESSAGE_VARARGS(name) \
static void name(fts_object_t *o, int winlet, fts_symbol_t s, \
         int ac, const fts_atom_t *at)

/* pass method arguments down to called method */
#define FTM_MESSAGE_ARGS	o, winlet, s, ac, at
/* alternative: FTM_CALL_METHOD(name) name(o, s, ac, at) */

/* add methods */
#define ftm_mess_bang(meth) \
fts_class_inlet_bang(cl, 0, meth)

#define ftm_mess_int(mess, meth) \
fts_class_message_int(cl, fts_new_symbol(mess), meth)

#define ftm_mess_float(mess, meth) \
fts_class_message_float(cl, fts_new_symbol(mess), meth)

#define ftm_mess_number(mess, meth) \
fts_class_message_number(cl, fts_new_symbol(mess), meth)

#define ftm_mess_varargs(mess, meth) \
fts_class_message_varargs(cl, fts_new_symbol(mess), meth)


/*
 * inlets and outlets 
 */

#define FTM_OUTLETS(num)	/* empty for jmax */


/* set number of outlets */
#define ftm_set_num_outlets(o, i) \
fts_object_set_outlets_number((fts_object_t *) o, i)

/* add outlets */
#define ftm_add_outlet_number(o, i) \
fts_class_outlet_number(fts_object_get_class((fts_object_t *) o), i)

/* send from outlets */
#define ftm_outlet_float(o, i, f) \
fts_outlet_float(o, i, f)


#endif	/* _JMAX_FTMEXT_H_ */

