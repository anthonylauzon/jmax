
package ircam.jmax.editors.sequence.track;

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.*;
import java.io.File;
import javax.swing.*;

/**
 * The EventValue object that represents a fricative event. Is used during score-recognition */
public class FricativeValue extends AbstractEventValue
{
    public FricativeValue()
    {
	super();
    }


    public ValueInfo getValueInfo()
    {
	return info;
    }


    static class FricativeValueInfo extends AbstractValueInfo {    
	/**
	 * Returns the name of this value object */
	public String getName()
	{
	    return FRICATIVE_NAME;
	}

	public ImageIcon getIcon()
	{
	    return FRICATIVE_ICON;
	}

	public Object newInstance()
	{
	    return new FricativeValue();
	}

	//---

    }


    /**
     * Returns its specialized renderer (a FricativeEventRenderer) */
    public ObjectRenderer getRenderer()
    {
	return FricativeEventRenderer.getRenderer();
    }

  
    //--- Fields

    public static final String FRICATIVE_NAME = "Fricative";
    static String path = MaxApplication.getProperty("sequencePackageDir")+File.separator+"images" +File.separator;

    public static ImageIcon FRICATIVE_ICON = new ImageIcon(path+"fricative.gif");
    public static FricativeValueInfo info = new FricativeValueInfo();
}
