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
 */

#include <fts/fts.h>
#include <string.h>

static fts_symbol_t sym_arch;

typedef struct 
{
  fts_object_t _o;

} sysinfo_t;


static void
sysinfo_arch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_symbol(o, 0, aym_arch);
}

static void
sysinfo_classes(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_package_t *pkg;
  fts_metaclass_t *mcl;
  fts_iterator_t pkg_iter;
  fts_atom_t pkg_name;

  /* ask the kernel package before any other package. The kernel
     classes should not be redefined anyway. If we search the kernel
     package before the required packages, we avoid the loading of all
     (required) packages to find the patcher class.  */
  pkg = fts_get_system_package();

  /* ask the required packages of the current package */
  fts_package_get_required_packages(pkg, &pkg_iter);

  while ( fts_iterator_has_more( &pkg_iter)) 
    {
      fts_iterator_next( &pkg_iter, &pkg_name);
      pkg = fts_package_get(fts_get_symbol(&pkg_name));

      fprintf(stderr, "package %s\n", pkg_name);
      
      if (pkg == NULL)
	continue;
    }
}

static fts_status_t
sysinfo_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sysinfo_t), 1, 1, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, sysinfo_arch);
  fts_method_define_varargs(cl, 0, fts_new_symbol("classes"), sysinfo_classes);
  
  return fts_Success;
}

void
sysinfo_config(void)
{
  sym_arch = fts_new_symbol(FTS_ARCH_NAME);

  fts_class_install(fts_new_symbol("sysinfo"), sysinfo_instantiate);
}
