#ifndef _OBJTABLE_H_
#define _OBJTABLE_H_
/* New version of the objtable.

   This version imply that the object IDs are assigned
   consecutively, starting from 0; any other client behaviour
   will eat enourmous amount of memory, and possibly will
   just kill the machine.


   The data structure is direct access table, adaptively resized 
   when needed.

   Register generate an ID for the object, and add it to the table;
   put require an id; put should only be used for server request,
   no ID should be generated in this case.
   register generate even numbers, the server should generate odd
   numbers !!!
   */

extern void fts_object_table_register(fts_object_t *obj);
extern void fts_object_table_put(int id, fts_object_t *obj);
extern void fts_object_table_remove(int id);
extern fts_object_t *fts_object_table_get(int id);
extern void fts_object_table_delete_all(void);

#endif
