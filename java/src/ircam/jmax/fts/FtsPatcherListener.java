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

import ircam.ftsclient.*;

/** This interface define a listener of the changes
  inside a patcher. It is installed on a FtsPatcherData instance
  */

public interface FtsPatcherListener
{
    //public void objectAdded(FtsPatcherObject patch, FtsObject object);
    public void objectAdded(FtsPatcherObject patch, FtsGraphicObject object, boolean doedit);
    public void objectRedefined(FtsPatcherObject patch, FtsGraphicObject newObject);
    public void objectRemoved(FtsPatcherObject patch, FtsGraphicObject object);
      
    public void connectionAdded(FtsPatcherObject patch, FtsConnection connection);
    public void connectionRemoved(FtsPatcherObject patch, FtsConnection connection);

    public void patcherChangedNumberOfInlets(FtsPatcherObject patch, int nins);
    public void patcherChangedNumberOfOutlets(FtsPatcherObject patch, int nouts);
    public void patcherChanged(FtsPatcherObject patch);

    public void patcherHaveMessage(String msg);
}
