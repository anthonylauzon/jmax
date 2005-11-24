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

package ircam.jmax.editors.sequence.renderers;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.*;

import ircam.jmax.editors.sequence.*;
/**
* A class used to register ValueInfo and acces it by name*/

public class SequenceImages {
  
  public static void init()
  {
    if(JMaxApplication.getProperty("ftm") == null)
    {
      String fs = File.separator;
      String path = JMaxApplication.getProperty("jmaxRoot")+fs+"packages"+fs+"sequence"+fs+"images"+fs;
      
      imageIcons.put("opened_track", new ImageIcon(path+"seq_opened_track_arrow.gif"));
      imageIcons.put("closed_track", new ImageIcon(path+"seq_closed_track_arrow.gif"));
      imageIcons.put("unselected_track", new ImageIcon(path+"seq_unselected_track.gif"));
      imageIcons.put("selected_track", new ImageIcon(path+"seq_selected_track.gif"));
      imageIcons.put("unmute", new ImageIcon(path+"seq_unmute.gif"));
      imageIcons.put("mute", new ImageIcon(path+"seq_mute.gif"));
    }
    else
    {
      imageIcons.put("opened_track", JMaxUtilities.loadIconFromResource("images/seq_opened_track_arrow.gif"));
      imageIcons.put("closed_track", JMaxUtilities.loadIconFromResource("images/seq_closed_track_arrow.gif"));
      imageIcons.put("unselected_track", JMaxUtilities.loadIconFromResource("images/seq_unselected_track.gif"));
      imageIcons.put("selected_track", JMaxUtilities.loadIconFromResource("images/seq_selected_track.gif"));
      imageIcons.put("unmute", JMaxUtilities.loadIconFromResource("images/seq_unmute.gif"));
      imageIcons.put("mute", JMaxUtilities.loadIconFromResource("images/seq_mute.gif"));
    }
  }
public static Image getImage(String name)
{
	return (Image)(images.get(name));
}
public static ImageIcon getImageIcon(String name)
{
	return (ImageIcon)(imageIcons.get(name));
}

//---
private static Hashtable images = new Hashtable();
private static Hashtable imageIcons = new Hashtable();
}
