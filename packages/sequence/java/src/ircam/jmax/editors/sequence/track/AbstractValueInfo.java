
package ircam.jmax.editors.sequence.track;

import ircam.jmax.*;
import java.awt.Component;
import ircam.jmax.editors.sequence.SequenceGraphicContext;
import javax.swing.*;
import java.io.File;

/**
 * A convenience implementation of the ValueInfo interface.
 */
public class AbstractValueInfo implements ValueInfo {
    
    public String getName()
    {
	return "unknown";
    }

    public ImageIcon getIcon()
    {
	return GENERIC_ICON;
    }
    
    public Object newInstance()
    {
	return new AbstractEventValue();
    }
    

    public ValueEditor newValueEditor()
    {
	return NullEditor.getInstance();
    }
    
    public Component newWidget(SequenceGraphicContext gc)
    {
	return null;
    }

    public AbstractValueInfo getInstance()
    {
	return instance;
    }

    //--- AbstractValueInfo fields
    public static AbstractValueInfo instance = new AbstractValueInfo();
    static String path = MaxApplication.getProperty("sequencePackageDir")+File.separator+"images" +File.separator;
    public static ImageIcon GENERIC_ICON = new ImageIcon(path+"genericAdder.gif");
}


