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

/**
 * This interface express the ability, for the implementing classes,
 * to switch the graphic context of the toolkit interaction.
 * Unlike the fixed paradigm (One editor instance = one graphic context = one window),
 * these objects can trigger a context-switch in situation not necessarily
 * related to windowActivate events.
 * These objects are somehow "active objects", they are usually listening
 * the triggering source (ex., the ComponentContextSwitcher class listens to mouseEntered events
 * in a given Component) and communicate the change to listeners. 
 * The user must prevent triggering conflicts (i.e. the context should be 
 * in every moment uniquely determined).
 */

public interface ContextSwitcher {

    /*
     * Ask to be informed by this switcher when Graphic context changes */
    abstract public void addSwitchListener(SwitchListener listener);
    
    /**
     * Remove the given listener */
    abstract public void removeSwitchListener(SwitchListener listener);

    /**
     * Get the context associated to this switcher*/
    public abstract GraphicContext getGraphicContext();


    /**
     * Sets the graphic context associated to this switcher. */
    public abstract void setGraphicContext(GraphicContext gc);
}

