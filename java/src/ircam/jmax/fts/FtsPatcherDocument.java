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

package ircam.jmax.fts; 

import java.io.*;


import ircam.jmax.*;
import ircam.jmax.mda.*;

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
    ((FtsPatcherData) data).getContainerObject().setDocument(this);
  }

  public void dispose()
  {
    super.dispose();

    FtsObject patcher;

    patcher = ((FtsPatcherData) getRootData()).getContainerObject();
    patcher.delete();
  }
}






