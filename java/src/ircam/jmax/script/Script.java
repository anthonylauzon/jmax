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

package ircam.jmax.script;

/**
 * Interface Script
 *
 * An interface for all scripts. To convert a script String to a
 * Script use Interp.convert(). 
 */
public interface Script 
{
    /** Evaluate the script. */
    public Object eval() throws ScriptException;

    /** Evaluate the script using the given arguments. If you only
     *  need to pass 1, 2, 3, or 4 arguments you can use one of the
     *  other eval methods. */
    public Object eval(Object[] arg) throws ScriptException;

    /** Evaluate the script with one aruments. */
    public Object eval(Object a1) throws ScriptException;

    /** Evaluate the script with two aruments. */
    public Object eval(Object a1, Object a2) throws ScriptException;

    /** Evaluate the script with three aruments. */
    public Object eval(Object a1, Object a2, Object a3) throws ScriptException;

    /** Evaluate the script with four aruments. */
    public Object eval(Object a1, Object a2, Object a3, Object a4) throws ScriptException;
}
