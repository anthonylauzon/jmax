/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <fts/fts.h>
#include <string.h>

extern fts_package_t* fts_get_system_package(void);

typedef struct 
{
  fts_object_t _o;
} sysinfo_t;

extern fts_class_t *fts_package_get_class(fts_package_t* pkg, fts_symbol_t name);
extern void fts_class_instantiate(fts_class_t *cl);

static void
sysinfo_classes(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t *pkg;
  fts_iterator_t pkg_iter;
  fts_atom_t pkg_name;

  /* post html header */
  fprintf(stderr, "<html>\n");
  fprintf(stderr, "<head>\n");
  fprintf(stderr, "  <title>jMax classes</title\n");
  fprintf(stderr, "</head>\n");
  fprintf(stderr, "<body>\n");
  
  /* head line */
  fprintf(stderr, "<h1>jMax classes</h1>\n");

  /* table header */
  fprintf(stderr, "<table width=100%% border=1 cellpadding=3 cellspacing=0>\n");
  fprintf(stderr, "<tr><td width=10%%>name</td><td width=2%%>ins</td><td width=2%%>outs</td><td>&nbsp;</td></tr>\n");

  fts_get_package_names(&pkg_iter);
  while (fts_iterator_has_more(&pkg_iter))
  {
    fts_iterator_next(&pkg_iter, &pkg_name);
    pkg = fts_package_get(fts_get_symbol(&pkg_name));

    if(pkg != NULL && fts_package_get_state(pkg) != fts_package_corrupt)
    {
      fts_iterator_t cl_iter;

      fprintf(stderr, "  <tr><td colspan=4><h2>%s</h2></td></tr>\n", fts_get_symbol(&pkg_name));

      fts_package_get_class_names(pkg, &cl_iter);
      while(fts_iterator_has_more(&cl_iter))
      {
        fts_atom_t a;
        fts_symbol_t name;
        fts_class_t *cl;

        fts_iterator_next(&cl_iter, &a);
        name = fts_get_symbol(&a);
        cl = fts_package_get_class(pkg, name);
        
        if (!cl->size)
          fts_class_instantiate(cl);
        
        fprintf(stderr, "    <tr><td>%s</td><td>%d</td><td>%d</td><td>&nbsp;</td></tr>\n", name, cl->ninlets, cl->noutlets);
      }
    }
  }

  /* table footer */
  fprintf(stderr, "</table>\n");

  /* post html footer */
  fprintf(stderr, "</body>\n");
  fprintf(stderr, "</html>\n");
}

static void
sysinfo_audio(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
#warning sysinfo_audio use fts_audioport_get_default, need to be changed
/*   fts_audioport_t *port = fts_audioport_get_default(o); */

/*   fts_send_message_varargs((fts_object_t *)port, fts_s_print, ac, at); */
}

static void
sysinfo_midi(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_t *config = fts_midiconfig_get();

  fts_send_message_varargs(config, fts_s_print, ac, at);
}

static void
sysinfo_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sysinfo_t), 0, 0);

  fts_class_message_varargs(cl, fts_new_symbol("classes"), sysinfo_classes);
  fts_class_message_varargs(cl, fts_new_symbol("audio"), sysinfo_audio);
  fts_class_message_varargs(cl, fts_new_symbol("midi"), sysinfo_midi);

  fts_class_inlet_thru(cl, 0);
}

void
sysinfo_config(void)
{
  fts_class_install(fts_new_symbol("sysinfo"), sysinfo_instantiate);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
