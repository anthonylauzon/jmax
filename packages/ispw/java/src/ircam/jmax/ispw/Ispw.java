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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.ispw;

import ircam.jmax.*;

/**
 * The table extension; install the table data type
 * and the table file data handler
 */
public class Ispw extends JMaxPackage
{
  public Ispw()
  {
      super("ispw");
  }

  public void load()
  {
      ObjectCreatorManager.registerFtsClass("messbox", ircam.jmax.ispw.FtsMessageObject.class);
      ObjectCreatorManager.registerGraphicClass("messbox", ircam.jmax.ispw.Message.class, "ispw");
  
      ircam.jmax.editors.console.ConsoleWindow.append("package Ispw loaded");
  }
}




