/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <fts/fts.h>
#include <fts/packages/data/data.h>
#include <string.h>

#include <ftsconfig.h>

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

extern void vec_config(void);
extern void ivec_config(void);
extern void fvec_config(void);
extern void cvec_config(void);
extern void mat_config(void);
extern void fmat_config(void);
extern void col_config(void);
extern void row_config(void);
extern void bpf_config(void);
extern void dict_config(void);
extern void tabeditor_config(void);

extern void getrange_config(void);

extern void dumpfile_config(void);

void
data_object_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_object(o, 0, o);
}

/***********************************************************************
 *
 * expression class
 *
 */
fts_class_t *expr_class = NULL;

static void
expr_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_t *self = (expr_t *) o;
  fts_symbol_t sym = fts_get_symbol(at);
  const char *str = fts_symbol_name(sym);
  int len = strlen(str);
  char *buff = (char *)alloca(sizeof(char) * len + 2);
  fts_tokenizer_t tokenizer;
  fts_status_t status;
  fts_atom_t a;
  
  strcpy(buff, str);
  buff[len + 1] = '\0'; /* tokenizer needs double null termination */
  
  fts_tokenizer_init_buffer(&tokenizer, buff, len + 2);
  
  while(fts_tokenizer_next(&tokenizer, &a) != 0)
    fts_array_append(&self->descr, 1, &a);
  
  fts_tokenizer_destroy(&tokenizer);
  
  status = fts_expression_set(self->expression, fts_array_get_size(&self->descr), fts_array_get_atoms(&self->descr));
  
  if(status != fts_ok)
  {
    fts_object_error(o, fts_status_get_description(status));
    fts_expression_delete(self->expression);
    self->expression = NULL;
  }
  
  self->status = fts_ok;  
}

static fts_status_t
expr_method_callback(int ac, const fts_atom_t *at, void *o)
{
  if(ac > 1)
  {
    fts_object_t *tup = fts_object_create(fts_tuple_class, ac, at);
    fts_return_object(tup);
  }
  else if(ac > 0)
    fts_return((fts_atom_t *)at);
  
  return fts_ok;
}

static void
_expr_evaluate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_t *self = (expr_t *)o;
  fts_hashtable_t *locals = NULL;
  
  if(fts_is_a(at, dict_type))
  {
    dict_t *dict = (dict_t *)fts_get_object(at);
    locals = dict_get_hashtable(dict);
    
    ac--;
    at++;
  }
  
  if(self->expression != NULL)
  {
    fts_status_t status = fts_expression_evaluate(self->expression, locals, fts_get_global_definitions(), ac, at, expr_method_callback, NULL);
    
    if(status != fts_ok)
      fts_object_error(o, fts_status_get_description(status));
    
    self->status = status;
  }
}

static fts_status_t
expr_function_callback(int ac, const fts_atom_t *at, void *o)
{
  fts_atom_t *a = (fts_atom_t *)o;
  
  if(ac > 1)
  {
    fts_object_t *tup = fts_object_create(fts_tuple_class, ac, at);
    fts_set_object(a, tup);
  }
  else if(ac > 0)
    *a = *at;
  
  return fts_ok;
}

void
expr_evaluate_in_scope(expr_t *self, fts_patcher_t *scope, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(scope == NULL)
    scope = fts_get_root_patcher();
  
  if(self->expression != NULL)
  {
    fts_status_t status = fts_expression_reduce(self->expression, scope, ac, at, expr_function_callback, (void *)ret);
    
    if(status != fts_ok)
      fts_set_void(ret);
    
    self->status = status;
  }
}

void
expr_evaluate(expr_t *self, fts_hashtable_t *locals, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(self->expression != NULL)
  {
    fts_status_t status = fts_expression_evaluate(self->expression, locals, fts_get_global_definitions(), ac, at, expr_function_callback, (void *)ret);
    
    if(status != fts_ok)
      fts_set_void(ret);
    
    self->status = status;
  }
}

static void
expr_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_t *self = (expr_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  
  fts_spost(stream, "<expr '");
  fts_spost_atoms(stream, fts_array_get_size(&self->descr), fts_array_get_atoms(&self->descr));
  fts_spost(stream, "'>");
}

static void
expr_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_post(o, 0, NULL, 0, NULL);
  fts_post("\n");
}

static void
expr_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_t *self = (expr_t *)o;
  
  self->expression = NULL;
  fts_array_init(&self->descr, 0, 0);  
  
  if(ac > 0 && fts_is_symbol(at))
  {
    fts_expression_new(0, 0, &self->expression);
    expr_set(o, 0, 0, ac, at);
  }
  else
    fts_object_error(o, "expression argument required");
}

static void
expr_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  expr_t *self = (expr_t *)o;
  
  fts_array_destroy(&self->descr);
  
  if(self->expression != NULL)
    fts_expression_delete(self->expression);
}

static void
expr_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(expr_t), expr_init, expr_delete);
  
  fts_class_message_varargs(cl, fts_s_post, expr_post);
  fts_class_message_varargs(cl, fts_s_print, expr_print);
  
  fts_class_message_varargs(cl, fts_s_name, fts_object_name);
  
  fts_class_message_varargs(cl, fts_new_symbol("eval"), _expr_evaluate);
  /*fts_class_message_symbol(cl, fts_new_symbol("set"), expr_set);*/
  
  fts_class_doc(cl, fts_s_print, NULL, "print expression");
}

/***********************************************************************
 *
 *  config
 *
 */
void
data_config(void)
{
  expr_class = fts_class_install(fts_new_symbol("expr"), expr_instantiate);
  
  vec_config();
  ivec_config();
  fvec_config();
  cvec_config();
  mat_config();
  fmat_config();
  col_config();
  row_config();
  bpf_config();
  dict_config();
  tabeditor_config();

  getrange_config();  

  dumpfile_config();
}
