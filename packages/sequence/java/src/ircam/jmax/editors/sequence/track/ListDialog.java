
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

class ListDialog extends JDialog implements TrackDataListener, TrackListListener, PopupProvider, ListContainer{
    
    ListDialog(Track track, Frame frame, SequenceGraphicContext gc)
    {
	super(frame, "list dialog: track <"+track.getName()+">", false);
	this.frame = frame;
	this.track = track;

	JPanel panel = new JPanel();
	panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
		
	list = new ListPanel(track, this, this, gc);
	scroll = new JScrollPane(list, JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, 
					     JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
	scroll.setBackground(Color.white);

	panel.add(initLabelsPanel());
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

    JPanel initLabelsPanel()
    {
	JPanel labelPanel = new JPanel();
	labelPanel.setLayout(new BoxLayout(labelPanel, BoxLayout.X_AXIS));
	
	String name;
	JLabel aLabel;

	aLabel = new JLabel("time" , JLabel.LEFT);
	aLabel.setPreferredSize(new Dimension(ListPanel.xstep , 20));
	labelPanel.add(aLabel);

	for(Enumeration e = track.getTrackDataModel().getPropertyNames(); e.hasMoreElements();)
	    {
		name = (String)e.nextElement();
		aLabel = new JLabel(name , JLabel.LEFT);
		aLabel.setPreferredSize(new Dimension(ListPanel.xstep , 20));
		labelPanel.add(aLabel);
	    }
	labelPanel.add(Box.createHorizontalGlue());
	
	applyButton = new JButton("apply");
	applyButton.setPreferredSize(new Dimension(60, 20));
	applyButton.setMaximumSize(new Dimension(60, 20));
	applyButton.setMargin(new Insets(0, 0, 0, 0));
	//applyButton.setVisible(false);
	applyButton.setEnabled(false);
	applyButton.addActionListener(new ActionListener(){
	    public void actionPerformed(ActionEvent e)
		{
		    list.setEventValue();
		}
	});

	labelPanel.add(applyButton);
	labelPanel.add(Box.createRigidArea(new Dimension(3, 10)));

	return labelPanel;
    }

    /**
     * TrackDataListener interface
     */
    public void objectChanged(Object spec, String propName, Object propValue) 
    {
	scroll.validate();
    }
    
    public void objectAdded(Object spec, int index) 
    {
	scroll.validate();
    }

    public void objectsAdded(int maxTime) 
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
    
    /**
     * PopupProvider interface
     */
    public JPopupMenu getMenu()
    {
	ListPopupMenu.update(list);
	return ListPopupMenu.getInstance();
    }
    /**
     * ListContainer interface
     */
    public boolean isVisible(int y)
    {
	Rectangle r = scroll.getViewport().getViewRect();
	return ((y >= r.y)&&(y <= r.y +r.height));
    }
    public Rectangle getVisibleRect()
    {
      return scroll.getViewport().getViewRect();
    }
    public void setEdit(boolean isEdit)
    {
	applyButton.setEnabled(isEdit);
	//applyButton.setVisible(isEdit);
    }

    Track track;
    ListPanel list;
    JScrollPane scroll;
    JButton applyButton;
    Frame frame;
}





