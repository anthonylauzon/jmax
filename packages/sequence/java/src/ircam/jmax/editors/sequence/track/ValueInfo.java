
package ircam.jmax.editors.sequence.track;

import java.awt.*;
import javax.swing.*;
import ircam.jmax.editors.sequence.*;
import java.util.*;

/**
 * A ValueInfo contains a set of informations about a specific EventValue,
 * such as its name, and a set of functionalities to
 * create new instances, new editor and widgets.
 */
public interface ValueInfo {
    
    /**
     * Returns the name of the Value */
    abstract public String getName();

    abstract public String getPublicName();

    abstract public ImageIcon getIcon();

    /**
     * Creates a new instance of the Value */
    abstract public Object newInstance(); 

    /**
     * Creates a new instance of an editor for this value, if any.
     * Such an editor would be used into generic, external, containers */
    abstract public ValueEditor newValueEditor();

    /**
     * Creates a widget (an editor for this value) that will be
     * used inside the Sequencer editor */
    abstract public Component newWidget(SequenceGraphicContext gc);

    abstract public Enumeration getPropertyNames();
    abstract public int getPropertyCount();
}



