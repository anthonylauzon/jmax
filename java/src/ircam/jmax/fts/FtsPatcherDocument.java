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

package ircam.jmax.fts; 

import java.io.*;


import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;

/** A FtsPatcherDocument is the Max Document containing an FTS Patch file.
 */

public class FtsPatcherDocument extends MaxDocument 
{
  public FtsPatcherDocument(MaxContext context)
  {
    super(context, Mda.getDocumentTypeByName("patcher"));
  }

  public void setRootData(MaxData data)
  {
      super.setRootData(data);
      ((FtsPatcherObject) data).setDocument(this);
  }

  public void dispose()
  {
    super.dispose();

    FtsObject patcher;

    patcher = (FtsPatcherObject) getRootData();
    patcher.delete();
  }
    
  public void edit() throws MaxDocumentException
  {
    if (rootData != null)
	((FtsPatcherObject) getRootData()).sendMessage(FtsObject.systemInlet, "open_editor");
    else
	super.edit();
  }
}






