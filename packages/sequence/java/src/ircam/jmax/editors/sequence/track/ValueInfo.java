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

package ircam.jmax.editors.sequence.track;

import java.awt.*;
import javax.swing.*;
import ircam.jmax.editors.sequence.*;
import java.awt.datatransfer.*;
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

    abstract public DataFlavor getDataFlavor();

    abstract public Enumeration getPropertyNames();
    abstract public int getPropertyCount();
    abstract public Class getPropertyType(int i);    
}



