
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
	ImageObserver observer =  new ImageObserver(){
	    public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height)
	    {
		return true;
	    }
	};
	
	//Create the backgrounds images????
	try{
	  path  = MaxApplication.getPackageHandler().locatePackage("sequence").getPath()+File.separator+"images"+File.separator;
	}
	catch(FileNotFoundException e){
	  path = MaxApplication.getProperty("sequencePackageDir")+File.separator+"images"+File.separator;
	}

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
