
package ircam.jmax.editors.sequence.track;

import java.awt.*;
import java.io.*;
import java.awt.event.*;
import javax.swing.*;
import ircam.jmax.MaxApplication;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.menus.*;

/**
 **/
public class ToggleBar extends /*JPanel*/PopupToolbarPanel
{
    public ToggleBar(TrackEditor trkEd)
    {
	super();
	trackEditor = trkEd;
	track = trackEditor.getTrack();
	setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	String fs = File.separator;
	String path = null;

	try
	    {
		path  = MaxApplication.getPackageHandler().locatePackage("sequence").getPath()+fs+"images"+fs;
	    }
	catch(FileNotFoundException e){
	    //System.err.println("Couldn't locate sequence images");
	    path = MaxApplication.getProperty("sequencePackageDir")+File.separator+"images"+File.separator;
	}

	openButton = new JButton(new ImageIcon(path+"closed_track_arrow.gif"));
	openButton.setPreferredSize(new Dimension(TrackContainer.BUTTON_WIDTH, TOGGLEBAR_HEIGHT+1));
	openButton.setMaximumSize(new Dimension(TrackContainer.BUTTON_WIDTH, TOGGLEBAR_HEIGHT+1));
	openButton.setMinimumSize(new Dimension(TrackContainer.BUTTON_WIDTH, TOGGLEBAR_HEIGHT+1));

	barButton = new JButton();
	barButton.setHorizontalAlignment(AbstractButton.LEFT);
	barButton.setForeground(Color.darkGray);
	barButton.setMargin(new Insets(0, 7, 0, 100));
	barButton.setFont(toggleBarFont);
	barButton.setText(track.getName());
	
	barButton.setPreferredSize(new Dimension(TOGGLEBAR_DEFAULT_WIDTH-TrackContainer.BUTTON_WIDTH, TOGGLEBAR_HEIGHT));

	add(openButton);
	add(barButton);

	barButton.addActionListener(new ActionListener(){
	    public void actionPerformed(ActionEvent e)
		{
		    track.setProperty("opened", Boolean.TRUE);
		    track.setProperty("active", Boolean.TRUE);
		}
	});
	openButton.addActionListener(new ActionListener(){
	    public void actionPerformed(ActionEvent e)
		{
		    track.setProperty("opened", Boolean.TRUE);
		    track.setProperty("active", Boolean.TRUE);
		}
	});
	barButton.addMouseListener(new MouseListener(){
	     public void mousePressed(MouseEvent e)
		 {
		     if (e.isPopupTrigger()) 
			 {
			     ToggleBar.this.doPopup(e);
			 }
		 }
	     public void mouseClicked(MouseEvent e){}
	     public void mouseReleased(MouseEvent e){}
	     public void mouseEntered(MouseEvent e){}
	     public void mouseExited(MouseEvent e){}
	});

	validate();
    }

    void doPopup(MouseEvent e)
    {
	processMouseEvent(e);
    }

    public Dimension getPreferredSize()
    {
	return new Dimension(TOGGLEBAR_DEFAULT_WIDTH, TOGGLEBAR_HEIGHT);
    }

    public JPopupMenu getMenu()
    {
	ClosedTrackPopupMenu.getInstance().update(trackEditor);
	return ClosedTrackPopupMenu.getInstance();
    }
    public static Font toggleBarFont = new Font("monospaced", Font.PLAIN, 10);
    boolean opened = true;
    JButton openButton, barButton;
    public static int TOGGLEBAR_HEIGHT = 14; 
    public static int TOGGLEBAR_DEFAULT_WIDTH = 1500; 
    Track track;
    TrackEditor trackEditor;
}












