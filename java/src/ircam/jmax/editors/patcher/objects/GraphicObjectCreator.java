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

package ircam.jmax.editors.patcher.objects;
/**
 * A specialized listener that is called when 
 * a new data value is available for an object
 * New  Data Listener are installed and remove locally 
 * in the Fts class; these listeners are private to the fts
 * package
 */
import ircam.jmax.editors.patcher.*;
import ircam.jmax.fts.*;
import javax.swing.*;
public interface GraphicObjectCreator
{
    public GraphicObject createInstance(ErmesSketchPad sketch, FtsObject object);
    public void init(String className, String iconName, String cursorName, String message);
    public String getClassName();
    public String getIconName();
    public ImageIcon getIcon();
    public String getCursorName();
    public String getMessage();
    public String getPackagePath();
}
