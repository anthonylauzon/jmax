#ifndef _DSP_H_
#define _DSP_H_

extern fts_module_t fts_dsp_module;

#define MINVS 8	      /* vectors must be a multiple of MINVS points */
#define DEFAULTVS 64
#define MAXVS 64

#define FTS_DSP_VECTOR_DEFAULT_SIZE DEFAULTVS

typedef struct {
  int id;
  fts_symbol_t name;
  int refcnt;
  int length;
  float srate;
} dsp_signal;

typedef struct {
  int ninputs, noutputs;
  dsp_signal **in, **out;
} fts_dsp_descr_t;

/* Macro to access the input and output characteristics
   from a dsp descriptor
   This macros are official part of the object DSP API, and their definition
   depend on the actual dsp structure.
 */

/* get input properties  */

#define fts_dsp_get_input_name(DESC, IN)   ((DESC)->in[(IN)]->name)
#define fts_dsp_get_input_size(DESC, IN)   ((DESC)->in[(IN)]->length)
#define fts_dsp_get_input_srate(DESC, IN)    ((DESC)->in[(IN)]->srate)

/* test for the null input special case */

#define fts_dsp_is_input_null(DESC, IN)   ((DESC)->in[(IN)]->id == 0)

/* get output properties  */

#define fts_dsp_get_output_name(DESC, OUT)   ((DESC)->out[(OUT)]->name)
#define fts_dsp_get_output_size(DESC, OUT)   ((DESC)->out[(OUT)]->length)
#define fts_dsp_get_output_srate(DESC, OUT)    ((DESC)->out[(OUT)]->srate)

/* End of macros */

extern dsp_signal * Sig_new(int vectorSize);
extern void	    Sig_free(dsp_signal *s);
extern void	    Sig_unreference(dsp_signal *s);
extern void	    Sig_reference(dsp_signal *s);
extern dsp_signal * Sig_getById(int id);
extern void	    Sig_print(dsp_signal *s);
extern void	    Sig_setup(int vectorSize);
extern int	    Sig_getCount(void);
extern int	    Sig_check(void);

extern void	    dsp_declare_function(fts_symbol_t name, ftl_wrapper_t fun);

extern void	    dsp_list_insert(fts_object_t *o);
extern void	    dsp_list_remove(fts_object_t *o);

extern void	    dsp_sig_inlet(fts_class_t *cl, int num);
extern void	    dsp_sig_outlet(fts_class_t *cl, int num);

extern void	    dsp_add_signal(fts_symbol_t name, int vs);
extern void	    dsp_add_funcall(fts_symbol_t , int, fts_atom_t *);

extern void	    dsp_chain_create(int vs);
extern void	    dsp_chain_delete(void);

extern void	    dsp_chain_post(void);
extern void         dsp_chain_post_signals(void);

extern void         dsp_chain_fprint(FILE *f);
extern void         dsp_chain_fprint_signals(FILE *f);


extern fts_object_t *dsp_get_current_object();

extern int          dsp_is_running( void);
extern void         dsp_chain_poll(void);
extern void         dsp_make_dsp_off_chain(void);
extern ftl_program_t *dsp_get_current_dsp_chain( void);

extern fts_symbol_t fts_s_put;
extern fts_symbol_t fts_s_sig;
extern fts_symbol_t fts_s_sig_zero;
extern fts_symbol_t fts_s_dsp_upsampling;
extern fts_symbol_t fts_s_dsp_downsampling;
extern fts_symbol_t fts_s_dsp_outputsize;
extern fts_symbol_t fts_s_dsp_descr;


#endif


