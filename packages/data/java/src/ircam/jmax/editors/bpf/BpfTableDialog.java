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

package ircam.jmax.editors.bpf;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import java.awt.event.*;

/**
 * The dialog associated to the LogicEditor class.
 * This dialog sets the "integer" property of the client TrackEvent.
 * */

class BpfTableDialog extends JDialog implements BpfDataListener {
    
    BpfTableDialog(Frame frame, BpfGraphicContext gc)
    {
	super(frame, "Bpf table dialog", false);
	this.frame = frame;

	BpfTableModel tabModel = new BpfTableModel(gc.getDataModel());
	tabPanel = new BpfTablePanel(tabModel, gc);

	getContentPane().add(tabPanel);
	
	gc.getDataModel().addBpfListener(this);

	relocate();

	setSize(300, 160);

	getContentPane().validate();
	tabPanel.validate();
	validate();
	pack();
    }

    /**
     * BpfDataListener interface
     */
    public void pointChanged(int oldIndex, int newIndex, float newTime, float newValue){}
    public void pointsChanged(){}
      
    public void pointAdded(int index) 
    {
	getContentPane().validate();
	tabPanel.validate();
	validate();
    }
      
    public void pointsDeleted(int index, int size) 
    {
	getContentPane().validate();
	tabPanel.validate();
	validate();
    }
    
    public void cleared()
    {
	getContentPane().validate();
	tabPanel.validate();
	validate();
    }

    public void relocate()
    {
	Rectangle b = frame.getBounds();
	setLocation(b.x+5, b.y+b.height);
    }

    BpfTablePanel tabPanel;
    Frame frame;
}







