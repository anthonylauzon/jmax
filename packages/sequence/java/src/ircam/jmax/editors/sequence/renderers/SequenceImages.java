
package ircam.jmax.editors.sequence.renderers;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.io.*;
import java.util.*;

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
    }

    public static Image getImage(String name)
    {
	return (Image)(images.get(name));
    }

    //---
    private static Hashtable images = new Hashtable();
}
