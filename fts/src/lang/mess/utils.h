
/* data atoms */
#define fts_data_atom_get_type(atom) ((fts_is_data(&atom))? (fts_data_get_class_name(fts_get_data(&atom))): (fts_get_type(&atom)))
extern void fts_data_atom_set(fts_atom_t *atom, fts_atom_t new);
extern void fts_data_atom_void(fts_atom_t *atom);

/* data -> atom list */
extern int fts_data_get_size(fts_data_t *data);
extern int fts_data_get_atoms(fts_data_t *data, int ac, fts_atom_t *at);

/* object utils */
extern void fts_method_define_data(fts_class_t *class, int winlet, fts_method_t fun);
extern void fts_outlet_data(fts_object_t *o, int woutlet, fts_data_t *data);
#define fts_outlet_data_atom(o, woutlet, atom) do{fts_outlet_send(o, woutlet, fts_data_atom_get_type(atom), 1, &atom);}while(0)

