
package ircam.jmax.editors.sequence.track;

import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.menus.*;
/**
 * The dialog associated to the LogicEditor class.
 * This dialog sets the "integer" property of the client TrackEvent.
 * */

class ListDialog extends JDialog implements TrackDataListener, TrackListListener, PopupProvider{
    
    ListDialog(Track track, Frame frame)
    {
	super(frame, "list dialog", false);
	this.frame = frame;
	this.track = track;

	JPanel panel = new JPanel();
	panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
	
	JPanel labelPanel = new JPanel();
	labelPanel.setLayout(new BoxLayout(labelPanel, BoxLayout.X_AXIS));
	JLabel aLabel = new JLabel(" time               pitch                duration", JLabel.LEFT);
	aLabel.setAlignmentX(Component.LEFT_ALIGNMENT);
	aLabel.setPreferredSize(new Dimension(300 , 20));
	labelPanel.add(aLabel);
	labelPanel.add(Box.createHorizontalGlue());

	panel.add(labelPanel);
	
	list = new ListPanel(track, this);
	scroll = new JScrollPane(list, JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, 
					     JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
	scroll.setBackground(Color.white);
	panel.add(scroll);

	getContentPane().add(panel);
	
	getContentPane().validate();
	
	validate();
	pack();

	track.getTrackDataModel().addListener(this);

	list.addListListener(this);

	setLocation(200, 200);
	Dimension dim = panel.getSize();
	if(dim.height+30>700) dim.height = 700;
	else dim.height += 30;
	setSize(dim);
    }

    /**
     * TrackDataListener interface
     */
    public void objectChanged(Object spec) 
    {
	scroll.validate();
    }
    
    public void objectAdded(Object spec, int index) 
    {
	scroll.validate();
    }
    
    public void objectDeleted(Object whichObject, int index) 
    {
	scroll.validate();
    }
    
    public void objectMoved(Object whichObject, int oldIndex, int newIndex) 
    {
	scroll.validate();
    }

    /**
     * TrackListListener  interface
     */
    public void eventSelected(int index, int paramIndex, TrackEvent evt)
    {
	scroll.repaint();	
    }
    public void eventDeselected(int index, TrackEvent evt)
    {
	scroll.repaint();	
    }
    
    public void showEvent(int index, TrackEvent evt, Rectangle r)
    {
	if((!scroll.getViewport().getViewRect().contains(r.x, r.y))||(!scroll.getViewport().getViewRect().contains(r.x, r.y+r.height)))
	    {
		scroll.getVerticalScrollBar().setValue(0);//??
		scroll.getViewport().scrollRectToVisible(r);
	    }
    }

    /**
     * PopupProvider interface
     */
    public JPopupMenu getMenu()
    {
	ListPopupMenu.update(list);
	return ListPopupMenu.getInstance();
    }

    Track track;
    ListPanel list;
    JScrollPane scroll;
    Frame frame;
}





