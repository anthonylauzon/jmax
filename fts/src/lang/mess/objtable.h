/* New version of the objtable.

   This version imply that the object IDs are assigned
   consecutively, starting from 0; any other client behaviour
   will eat enourmous amount of memory, and possibly will
   just kill the machine.

   Only one client is supported.

   The data structure is direct access table, adaptively resized 
   when needed.
   */


extern void fts_object_table_put(int id, fts_object_t *obj);
extern void fts_object_table_remove(int id);
extern fts_object_t *fts_object_table_get(int id);
extern void fts_object_table_delete_all(void);
