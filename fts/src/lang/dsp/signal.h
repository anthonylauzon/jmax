#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#define MINVS 8	      /* vectors must be a multiple of MINVS points */
#define DEFAULTVS 64
#define MAXVS 64

typedef struct {
  int id;
  fts_symbol_t name;
  int refcnt;
  int length;
  float srate;
} dsp_signal;

typedef struct {
  int ninputs, noutputs;
  dsp_signal *in, *out;
} dsp_ugen;

extern dsp_signal * Sig_new(int vectorSize);
extern void	    Sig_free(dsp_signal *s);
extern void	    Sig_unreference(int id);
extern void	    Sig_reference(int id);
extern dsp_signal * Sig_getById(int id);
extern void	    Sig_print(dsp_signal *s);
extern void	    Sig_setup(int vectorSize);
extern int	    Sig_getCount(void);
extern int	    Sig_check(void);
extern void         Sig_init(void);

#endif
