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
