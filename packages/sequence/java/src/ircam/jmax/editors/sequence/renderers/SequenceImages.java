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
	String path;
	Image image;
	String fs = File.separator;
	ImageObserver observer =  new ImageObserver(){
	    public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height)
	    {
		return true;
	    }
	};
	/*
	  WARNING:
	  Waiting for a method to get the packagePath from the package name
	*/
	//Create the backgrounds images????
	/*try{
	  path  = MaxApplication.getPackageHandler().locatePackage("sequence").getPath()+File.separator+"images"+File.separator;
	  }
	  catch(FileNotFoundException e){
	  path = JMaxApplication.getProperty("sequencePackageDir")+File.separator+"images"+File.separator;
	  }*/
	path = JMaxApplication.getProperty("jmaxRoot")+fs+"packages"+fs+"sequence"+fs+"images"+fs;//??????????????   
	/*************************************************************/

	image = Toolkit.getDefaultToolkit().getImage(path+"violinClef.gif");
	Toolkit.getDefaultToolkit().prepareImage(image, 20, 51, observer);
	images.put("violinClef", image);
	image = Toolkit.getDefaultToolkit().getImage(path+"faClef.gif");
	Toolkit.getDefaultToolkit().prepareImage(image, 23, 30, observer);
	images.put("faClef", image);

	imageIcons.put("opened_track", new ImageIcon(path+"opened_track_arrow.gif"));
	imageIcons.put("closed_track", new ImageIcon(path+"closed_track_arrow.gif"));
	imageIcons.put("unselected_track", new ImageIcon(path+"unselected_track.gif"));
	imageIcons.put("selected_track", new ImageIcon(path+"selected_track.gif"));
	imageIcons.put("unmute", new ImageIcon(path+"unmute.gif"));
	imageIcons.put("mute", new ImageIcon(path+"mute.gif"));
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
