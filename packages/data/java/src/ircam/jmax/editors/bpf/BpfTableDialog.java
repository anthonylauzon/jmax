
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
      
    public void pointsDeleted(int[] indexs) 
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







