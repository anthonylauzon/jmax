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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

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

    public PopupProvider getProvider()
    {
	return provider;
    }
    
    //--- Fields
    PopupProvider provider;
}
