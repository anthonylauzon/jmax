
package ircam.jmax.editors.sequence.track;
import java.awt.*;
import java.io.*;
import java.awt.event.*;
import javax.swing.*;
import ircam.jmax.MaxApplication;
import ircam.jmax.editors.sequence.*;

/**
 **/
public class ToggleBar extends JPanel
{
    public ToggleBar(Track trk)
    {
	super();
	track = trk;
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
	openButton.setPreferredSize(new Dimension(TrackContainer.BUTTON_WIDTH, 14));

	barButton = new JButton();
	barButton.setHorizontalAlignment(AbstractButton.LEFT);
	barButton.setForeground(Color.darkGray);
	barButton.setMargin(new Insets(0, 7, 0, 100));
	barButton.setFont(toggleBarFont);
	barButton.setText(track.getName());
	
	barButton.setPreferredSize(new Dimension(1000, 14));

	add(openButton);
	add(barButton);

	barButton.addActionListener(new ActionListener(){
	    public void actionPerformed(ActionEvent e)
		{
		    track.setProperty("opened", Boolean.TRUE);
		}
	});
	openButton.addActionListener(new ActionListener(){
	    public void actionPerformed(ActionEvent e)
		{
		    track.setProperty("opened", Boolean.TRUE);
		}
	});

	validate();
    }

    public Dimension getPreferredSize()
    {
	return new Dimension(1000, 14);
    }

    public static Font toggleBarFont = new Font("monospaced", Font.PLAIN, 10);
    boolean opened = true;
    JButton openButton, barButton;
    Track track;
}












