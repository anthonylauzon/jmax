//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax;

import java.io.*;

// import javax.swing.*;
import javax.swing.ImageIcon;

public class JMaxIcons {
  public static final ImageIcon jmax_patcher_file_ = loadIcon( "mini_icon_jmax.gif");
  public static final ImageIcon jmax_tiny_logo_ = loadIcon( "jmax_logo_tiny.gif");
  public static final ImageIcon jmaxIcon = loadIcon( "jmax_icon.gif");
  public static final ImageIcon lockMode = loadIcon( "tool_lock_mode.gif");
  public static final ImageIcon editMode = loadIcon( "tool_edit_mode.gif");
  public static final ImageIcon up = loadIcon( "tool_up.gif");
  public static final ImageIcon upSelected = loadIcon( "tool_up_selected.gif");
  public static final ImageIcon toSave = loadIcon( "to_save.gif");
  public static final ImageIcon toSaveSelected = loadIcon( "to_save_selected.gif");
  public static final ImageIcon dspOn = loadIcon( "dsp_on.gif");
  public static final ImageIcon dspOff = loadIcon( "dsp_off.gif");
  public static final ImageIcon dspUndef = loadIcon( "dsp_undef.gif");
  public static final ImageIcon moreObjects = loadIcon( "more_objects.gif");
  public static final ImageIcon audioin_on = loadIcon( "audioin_on.gif");
  public static final ImageIcon audioin_off = loadIcon( "audioin_off.gif");
  public static final ImageIcon no_audioin = loadIcon( "no_audioin.gif");

  // These are used from ErrorTablePanel ???
  public static final ImageIcon patcher = loadIcon( "tool_patcher.gif");
  public static final ImageIcon errorObject = loadIcon( "tool_err.gif");

  // These are used from ObjectSetViewer ???
  public static final ImageIcon object = loadIcon( "tool_ext.gif");
  public static final ImageIcon inlet = loadIcon( "tool_in.gif");
  public static final ImageIcon outlet = loadIcon( "tool_out.gif");

  // These are used from ProjectEditor ???
  public static final ImageIcon projectRoot = loadIcon( "project_root.gif");

  // These are used from IncrementController ???
  public static final ImageIcon little_up_arrow   = loadIcon( "little_up_arrow.gif");
  public static final ImageIcon little_down_arrow = loadIcon( "little_down_arrow.gif");

  private static ImageIcon loadIcon( String iconPath)
  {
    String fs = File.separator;
    String fullPath = ((String)JMaxApplication.getProperty( "jmaxRoot")) + fs + "images" + fs + iconPath;

    return new ImageIcon( fullPath);
  }
}
 
