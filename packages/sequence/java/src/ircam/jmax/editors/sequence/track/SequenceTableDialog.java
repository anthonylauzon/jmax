
package ircam.jmax.editors.sequence.track;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import java.awt.event.*;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.menus.*;
/**
 * The dialog associated to the LogicEditor class.
 * This dialog sets the "integer" property of the client TrackEvent.
 * */

class SequenceTableDialog extends JDialog implements TrackDataListener/*, TrackListListener, PopupProvider, ListContainer*/{
    
    SequenceTableDialog(Track track, Frame frame, SequenceGraphicContext gc)
    {
	super(frame, "table dialog: track <"+track.getName()+">", false);
	this.frame = frame;
	this.track = track;

	TrackTableModel tabModel = new TrackTableModel(track.getTrackDataModel());
	tabPanel = new SequenceTablePanel(tabModel, gc);

	getContentPane().add(tabPanel);
	
	getContentPane().validate();
	
	validate();
	pack();

	track.getTrackDataModel().addListener(this);

	setLocation(200, 200);
	Dimension dim = tabPanel.getSize();
	
	if(dim.height+30>700) dim.height = 700;
	else dim.height += 30;
	setSize(dim);
    }

    /**
     * TrackDataListener interface
     */
    public void objectChanged(Object spec, String propName, Object propValue){}
      
    public void objectAdded(Object spec, int index) 
    {
	getContentPane().validate();
	tabPanel.validate();
	validate();
	pack();
    }
      
    public void objectsAdded(int maxTime) 
    {
	getContentPane().validate();
	tabPanel.validate();
	validate();
	pack();
    }
    
    public void objectDeleted(Object whichObject, int index) 
    {
	getContentPane().validate();
	tabPanel.validate();
	validate();
	pack();
    }
    
    public void objectMoved(Object whichObject, int oldIndex, int newIndex){}
    public void trackNameChanged(String oldName, String newName){};

    /**
     * PopupProvider interface
     */
    /*public JPopupMenu getMenu()
      {
      ListPopupMenu.update(list);
      return ListPopupMenu.getInstance();
      }*/

    Track track;
    SequenceTablePanel tabPanel;
    Frame frame;
}





