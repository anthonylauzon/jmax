#ifndef _FTL_H_
#define _FTL_H_

extern fts_module_t fts_ftl_module;

typedef void (*ftl_wrapper_t)(fts_word_t *);

typedef struct _ftl_program_t ftl_program_t;

typedef struct _ftl_subroutine_t ftl_subroutine_t;


extern void ftl_program_init( ftl_program_t *prog);

extern ftl_program_t *ftl_program_new( void );

extern void ftl_program_destroy( ftl_program_t *prog);

extern void ftl_program_free( ftl_program_t *prog );


extern ftl_subroutine_t *ftl_program_add_subroutine( ftl_program_t *prog, fts_symbol_t name);

extern ftl_subroutine_t *ftl_program_add_main( ftl_program_t *prog);

extern ftl_subroutine_t *ftl_program_set_current_subroutine( ftl_program_t *prog, ftl_subroutine_t *subr);

extern fts_status_t ftl_program_add_call( ftl_program_t *prog, fts_symbol_t name, int argc, const fts_atom_t *argv, fts_object_t *object);

extern fts_status_t ftl_program_add_return( ftl_program_t *prog);

extern int ftl_program_add_signal( ftl_program_t *prog, fts_symbol_t name, int vector_size);

extern int ftl_declare_function( fts_symbol_t name, ftl_wrapper_t wrapper);


extern void ftl_program_post( const ftl_program_t *prog );
extern void ftl_program_post_signals( const ftl_program_t *prog );
extern void ftl_program_post_signals_count( const ftl_program_t *prog);
extern void ftl_program_post_bytecode( const ftl_program_t *prog);

extern void ftl_program_fprint( FILE *f, const ftl_program_t *prog );
extern void ftl_program_fprint_signals( FILE *f, const ftl_program_t *prog );
extern void ftl_program_fprint_signals_count( FILE *f, const ftl_program_t *prog);


extern int ftl_program_compile( ftl_program_t *prog);

extern void ftl_program_call_subr( ftl_program_t *prog, ftl_subroutine_t *subr);
extern void ftl_program_run( ftl_program_t *prog );

extern fts_object_t *ftl_program_get_current_object( ftl_program_t *prog);

#endif
