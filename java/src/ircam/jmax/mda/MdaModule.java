//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.mda;

/** The mda module; the init function is called at init time
 *  by jmax, and install module related things.
 */

public class MdaModule
{
  static public void initModule()
  {
    // Install the local mda entities

    // Mda.installDocumentHandler( new MaxTclFileDocumentHandler());

    Mda.installDocumentHandler( new MaxTclExecutedDocumentHandler());
    Mda.installDocumentType(new MaxTclExecutedDocumentType());

    // Install the tcl commands for Mda

    ircam.jmax.mda.tcl.TclMdaPackage.installPackage();
  }
}
