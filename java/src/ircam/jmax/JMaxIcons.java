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

package ircam.jmax;

import java.io.*;
import javax.swing.*;

public class JMaxIcons {
  public static final ImageIcon jmax_patcher_file_ = loadIcon( "mini_icon_jmax.gif");
  public static final ImageIcon jmax_tiny_logo_ = loadIcon( "jmax_logo_tiny.gif");
  public static final ImageIcon lockMode = loadIcon( "tool_lock_mode.gif");
  public static final ImageIcon editMode = loadIcon( "tool_edit_mode.gif");
  public static final ImageIcon up = loadIcon( "tool_up.gif");
  public static final ImageIcon toSave = loadIcon( "to_save.gif");
  public static final ImageIcon dspOn = loadIcon( "dsp_on.gif");
  public static final ImageIcon dspOff = loadIcon( "dsp_off.gif");
  public static final ImageIcon moreObjects = loadIcon( "more_objects.gif");

  // These are used from ErrorTablePanel ???
  public static final ImageIcon patcher = loadIcon( "tool_patcher.gif");
  public static final ImageIcon errorObject = loadIcon( "tool_err.gif");

  // These are used from ObjectSetViewer ???
  public static final ImageIcon object = loadIcon( "tool_ext.gif");
  public static final ImageIcon inlet = loadIcon( "tool_in.gif");
  public static final ImageIcon outlet = loadIcon( "tool_out.gif");

  private static ImageIcon loadIcon( String iconPath)
  {
    String fs = File.separator;
    String fullPath = ((String)JMaxApplication.getProperty( "jmaxRoot")) + fs + "images" + fs + iconPath;

    return new ImageIcon( fullPath);
  }
}
 
