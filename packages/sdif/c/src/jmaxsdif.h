
#include <fts/fts.h>
#include <sdif.h>

/* heuristically determine jmax version */
#ifdef FTS_VERSION_STRING
#   define JMAXSDIF_JMAX3 0
#   define jmaxsdif_get_time()	fts_get_time()
#else
#   define JMAXSDIF_JMAX3 1
#   define jmaxsdif_get_time()	fts_timebase_get_time(timebase)
#endif

#define JMAXSDIFVERSION "0_1"


typedef enum { state_stopped, state_running } jmaxsdif_state_t;

typedef struct
{
    fts_object_t	o;		/* mandatory fts base class */
    char *		objectname;	/* who are we? */
    jmaxsdif_state_t	state;
    char *		filename;
    SdifFileT *		file;		/* NULL if not open */
} jmaxsdif_t;


#if JMAXSDIF_JMAX3
extern fts_timebase_t *timebase;
#endif
extern fts_symbol_t readsdif_symbol;
extern fts_symbol_t writesdif_symbol;
extern fts_symbol_t s_define;
extern fts_symbol_t s_open;
extern fts_symbol_t s_record;
extern fts_symbol_t s_close;


/* prototypes */
void sdif_config(void);
void jmaxsdif_exit(void);
void jmaxsdif_errorhandler(int		    error_tag, 
			   SdifErrorLevelET error_level, 
			   char*	    error_message, 
			   SdifFileT*	    error_file, 
			   SdifErrorT*	    error_ptr, 
			   char*	    source_file, 
			   int		    source_line);
int  jmaxsdif_closefile(jmaxsdif_t *this);

fts_status_t readsdif_instantiate(fts_class_t *, int, const fts_atom_t *);
fts_status_t writesdif_instantiate(fts_class_t *, int, const fts_atom_t *);
