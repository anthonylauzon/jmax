
package ircam.jmax.toolkit;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

/** 
 * A context switcher that is listening to WindowEvents.
 * It fires a context changed when the corrisponding window is activated.
 * Closing the window generates a context destroyed event.
 * @see ComponentContextSwitcher
 * @see ContextSwitcher
 */
public class WindowContextSwitcher implements ContextSwitcher{

    /**
     * @param window, the window to listen to
     * @param gc, the graphic context associated 
     */
    public WindowContextSwitcher(Window window, GraphicContext gc)
    {
	this.window = window;
	this.gc = gc;

	window.addWindowListener( new WindowAdapter() {
	    public void windowActivated(WindowEvent e)
		{
		    notifyChanged(WindowContextSwitcher.this.gc);
		}

	    public void windowClosing(WindowEvent e)
		{
		    notifyDestroyed(WindowContextSwitcher.this.gc);
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

    /** Utility function to notificate to listeners */
    private void notifyDestroyed(GraphicContext gc)
    {
	SwitchListener l;

	for (Enumeration e=listeners.elements(); e.hasMoreElements();)
	    {
		l = (SwitchListener)e.nextElement();
		l.contextDestroyed(gc);
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

    //--- WindowContextSwitcher fields
    Window window;
    GraphicContext gc; 
    Vector listeners = new Vector();
}
