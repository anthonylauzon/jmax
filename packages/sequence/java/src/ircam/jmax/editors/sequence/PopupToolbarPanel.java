
package ircam.jmax.editors.sequence;

import javax.swing.*;
import java.awt.event.*;

/**
 * This is a convenience class used to factorize the code needed
 * to handle the "show popup" actions in a track (as well as
 * in every component that is interested in this functionality).
 * Since the showing of the popup must be handled by the (awt) component
 * in which the mouse event originates, we are forced to make this class
 * a graphic class (the processMouseEvent can't be delegated). We choose, arbitarly, to
 * derive from JPanel). 
 * This component can be used in two ways: 
 * <ul>
 *   <li> deriving from it (you must override the getMenu() method!!!)  </li>
 *   <li> constructing it with a provider (PopupProvider)                         </li>
 * </ul>
 */
public class PopupToolbarPanel extends JPanel implements PopupProvider{

    public PopupToolbarPanel()
    {
    }

    public PopupToolbarPanel(PopupProvider provider)
    {
	this.provider = provider;
    }

    protected void processMouseEvent(MouseEvent e)
    {
	if (e.isPopupTrigger()) 
	    {
		if (provider != null)
		    provider.getMenu().show (e.getComponent(), e.getX()-10, e.getY()-10);
		else getMenu().show (e.getComponent(), e.getX()-10, e.getY()-10);
	    }
	else 
	    {
		super.processMouseEvent(e);
	    }
    }

    public JPopupMenu getMenu()
    {
	return null;
    }

    //--- Fields
    PopupProvider provider;
}
