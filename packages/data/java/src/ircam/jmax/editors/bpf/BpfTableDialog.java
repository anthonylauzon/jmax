
package ircam.jmax.editors.bpf;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import java.awt.event.*;

/**
 * The dialog associated to the LogicEditor class.
 * This dialog sets the "integer" property of the client TrackEvent.
 * */

class BpfTableDialog extends JDialog implements BpfDataListener/*, ListContainer*/{
    
    BpfTableDialog(Frame frame, BpfGraphicContext gc)
    {
	super(frame, "Bpf table dialog", false);
	this.frame = frame;

	BpfTableModel tabModel = new BpfTableModel(gc.getDataModel());
	tabPanel = new BpfTablePanel(tabModel, gc);

	getContentPane().add(tabPanel);
	
	getContentPane().validate();
	
	validate();
	pack();

	gc.getDataModel().addBpfListener(this);

	setLocation(200, 200);
	Dimension dim = tabPanel.getSize();
	
	if(dim.height+30>700) dim.height = 700;
	else dim.height += 30;
	setSize(dim);
    }

    /**
     * BpfDataListener interface
     */
    public void pointChanged(int oldIndex, int newIndex, float newTime, float newValue){}
      
    public void pointAdded(int index) 
    {
	getContentPane().validate();
	tabPanel.validate();
	validate();
	pack();
    }
      
    public void pointsDeleted(int[] indexs) 
    {
	getContentPane().validate();
	tabPanel.validate();
	validate();
	pack();
    }
    
    public void cleared()
    {
	getContentPane().validate();
	tabPanel.validate();
	validate();
	pack();
    };

    BpfTablePanel tabPanel;
    Frame frame;
}







