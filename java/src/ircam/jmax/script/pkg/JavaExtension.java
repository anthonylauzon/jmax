//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
// 
// Author: Peter Hanappe
//

package ircam.jmax.script.pkg;
import ircam.jmax.script.*;

/**
 *  Interface JavaExtension
 *
 *  A class inside a jMax package that needs special initialisation
 *  should implement this interface.
 */
public interface JavaExtension 
{
    /**
     * The init method will be called when the class is loaded thru
     * the MaxPackage.loadClass mechanism. The class is generally loaded
     * after a "(load-class X)" call in the package's init file (.scm).
     *
     * @param interp The interpreter
     */
    public void init(Interpreter interp);
}
