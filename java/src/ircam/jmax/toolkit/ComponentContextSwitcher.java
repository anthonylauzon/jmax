
package ircam.jmax.toolkit;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

/** 
 * A context switcher that is listening to mouseEnter events in a generic Component.
 * It fires a context changed when the mouse enters the specified component.
 * @see WindowContextSwitcher
 * @see ContextSwitcher
 */

public class ComponentContextSwitcher implements ContextSwitcher{

    /**
     * @param component, the component to listen to
     * @param gc, the graphic context associated 
     */
    public ComponentContextSwitcher(Component component, GraphicContext gc)
    {
	this.component = component;
	this.gc = gc;

	component.addMouseListener( new MouseAdapter() {
	    public void mouseEntered(MouseEvent e)
		{
		    notifyChanged(ComponentContextSwitcher.this.gc);
		}

	});
    }


    /** Utility function to notificate to listeners */
    private void notifyChanged(GraphicContext gc)
    {
	SwitchListener l;

	for (Enumeration e=listeners.elements(); e.hasMoreElements();)
	    {
		l = (SwitchListener)e.nextElement();
		l.contextChanged(gc);
	    }
    }


    /*
     * Ask to be informed when Graphic context switch occurs */
    public void addSwitchListener(SwitchListener l)
    {
	listeners.addElement(l);
    }

    /**
     * Remove the given listener */
    public void removeSwitchListener(SwitchListener l)
    {
	listeners.removeElement(l);
    }


    /**
     * Returns the graphic context associated to this switcher */
    public GraphicContext getGraphicContext()
    {
	return gc;
    }

    /**
     * Sets the GraphicContext that this switcher must handle */
    public void setGraphicContext(GraphicContext gc)
    {
	this.gc = gc;
    }

    //--- ComponentContextSwitcher fields
    Component component;
    GraphicContext gc; 
    Vector listeners = new Vector();
}
