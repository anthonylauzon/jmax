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


package ircam.jmax;

import java.io.*;
import ircam.jmax.toolkit.*;
//=====================================================
//   Platform 
//=====================================================

public class DefaultIcons
{
  static public void init()
  { 
      String fs = File.separator;
      String path = ((String)MaxApplication.getProperty("jmaxRoot"))+fs+"images"+fs;
      SystemIcons.loadIcon("_jmax_patcher_file_", path+"mini_icon_jmax.gif");
      SystemIcons.loadIcon("_jmax_tiny_logo_", path+"jmax_logo_tiny.gif");
      SystemIcons.loadIcon("_patcher_", path+"tool_patcher.gif");
      SystemIcons.loadIcon("_object_", path+"tool_ext.gif");
      SystemIcons.loadIcon("_error_object_", path+"tool_err.gif");
      SystemIcons.loadIcon("_inlet_", path+"tool_in.gif");
      SystemIcons.loadIcon("_outlet_", path+"tool_out.gif");
      SystemIcons.loadIcon("_lock_mode_", path+"tool_lock_mode.gif");
      SystemIcons.loadIcon("_edit_mode_", path+"tool_edit_mode.gif");
      SystemIcons.loadIcon("_up_", path+"tool_up.gif");
      SystemIcons.loadIcon("_to_save_", path+"to_save.gif");
      SystemIcons.loadIcon("_dsp_on_", path+"dsp_on.gif");
      SystemIcons.loadIcon("_dsp_off_", path+"dsp_off.gif");
      SystemIcons.loadIcon("_more_objects_", path+"more_objects.gif");
      
      Icons.loadIcon("%jmax", path+"jmax_logo_tiny.gif");
      Icons.loadIcon("%new", path+"new.gif");
      Icons.loadIcon("%hot", path+"hot.gif");
      Icons.loadIcon("%stop", path+"stop.gif");
      Icons.loadIcon("%at_work", path+"at_work.gif");
      Icons.loadIcon("%question", path+"question.gif");
      Icons.loadIcon("%warning", path+"warning.gif");
      Icons.loadIcon("%info", path+"info.gif");
      Icons.loadIcon("%linux", path+"linux.gif");
      Icons.loadIcon("%forum", path+"forum.gif");
      Icons.loadIcon("%arrow_left", path+"arrow_left.gif");
      Icons.loadIcon("%arrow_right", path+"arrow_right.gif");
      Icons.loadIcon("%arrow_up", path+"arrow_up.gif");
      Icons.loadIcon("%arrow_down", path+"arrow_down.gif");
  }
}
 
