
package ircam.jmax.editors.sequence;

import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.io.*;
import javax.swing.*;
import javax.swing.border.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.MaxApplication;

/**
 * A graphic component that contains a single track editor*/
public class TrackContainer extends JPanel {

  public TrackContainer(Track t, TrackEditor trackEditor)
  {
    super();
    setLayout(new BorderLayout());
    
    this.track = t;
    this.trackEditor = trackEditor;
    
    // icons: 
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

    activationButton = new JToggleButton(new ImageIcon(path+"unselected_track.gif"));
    activationButton.setSelectedIcon(new ImageIcon(path+"selected_track.gif"));
    activationButton.setPreferredSize(new Dimension(BUTTON_WIDTH, 70));
    add(activationButton, BorderLayout.WEST);

    /////////////////////////
    openButton = new JButton(new ImageIcon(path+"opened_track_arrow.gif"));
    openButton.setPreferredSize(new Dimension(BUTTON_WIDTH, 20));

    buttonPanel = new JPanel();
    buttonPanel.setLayout(new BorderLayout());
    buttonPanel.add(openButton, BorderLayout.NORTH);
    buttonPanel.add(activationButton, BorderLayout.CENTER);

    add(buttonPanel, BorderLayout.WEST);

    add(trackEditor.getComponent(), BorderLayout.CENTER);
    
    // --- set the "active" property of the track when the button is pressed
    activationButton.addActionListener( new ActionListener() {
      public void actionPerformed(ActionEvent e)
	{
	  track.setProperty("active", Boolean.TRUE);
	}
    });

    openButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	    {
		track.setProperty("opened", Boolean.FALSE);
	    }
    });

    toggleBar = new ToggleBar(trackEditor);
    add(toggleBar, BorderLayout.NORTH);
    toggleBar.setVisible(false);
    
    // --- change the selected state of the button when the "active" property of the track changes
    track.getPropertySupport().addPropertyChangeListener( new ActiveListener(activationButton));
    // an inner class
  }


  /**
   * Returns the button used to activate this track.
   */
  public AbstractButton getActivationButton()
  {
    return activationButton;
  }

  /**
   * A listener of the "active" property of a track. Its role
   * is to switch the state of the activation button when
   * the active state changes 
   */
  class ActiveListener implements PropertyChangeListener {
    
    public ActiveListener(AbstractButton b)
    {
      this.b = b;
    }
    
    public void propertyChange(PropertyChangeEvent evt)
    {
      boolean active = false;	    
      boolean opened = true;	    
      String name = evt.getPropertyName();

      if (name.equals("active"))
	  {
	      active = ((Boolean) evt.getNewValue()).booleanValue();
	      b.setSelected(active);
	  }
      else if (name.equals("opened"))
	  {
	      opened = ((Boolean) evt.getNewValue()).booleanValue();

	      int height;
	      if(opened)
		  height = trackEditor.getDefaultHeight();
	      else
		  height = ToggleBar.TOGGLEBAR_HEIGHT+4;
	      
	      setPreferredSize(new Dimension(getPreferredSize().width, height));
	      setMaximumSize(new Dimension(getMaximumSize().width, height));

	      trackEditor.getComponent().setVisible(opened);
	      buttonPanel.setVisible(opened);
	      toggleBar.setVisible(!opened);

	      if(opened)
		  trackEditor.getGraphicContext().getFtsSequenceObject().changeTrack(track);
	      //validate();
	  }
      else 
	  if(name.equals("maximumPitch") || name.equals("minimumPitch"))
	      {
		  int height = ((PartitionAdapter)trackEditor.getGraphicContext().getAdapter()).getRangeHeight();
		  setSize(getSize().width, height);
		  setPreferredSize(new Dimension(getPreferredSize().width, height));
		  setMaximumSize(new Dimension(getMaximumSize().width, height));
		  trackEditor.getGraphicContext().getFtsSequenceObject().changeTrack(track);	  
	      }
	  else 
	      if(name.equals("viewMode"))
		 {
		     if(((Integer)evt.getNewValue()).intValue() < 2)//MidiTrack only
			 {
			     int height = ((PartitionAdapter)trackEditor.getGraphicContext().getAdapter()).getRangeHeight();
			     setSize(getSize().width, height);
			     setPreferredSize(new Dimension(getPreferredSize().width, height));
			     setMaximumSize(new Dimension(getMaximumSize().width, height));
			     trackEditor.getGraphicContext().getFtsSequenceObject().changeTrack(track);	  
			 }
		 }
	      else 
		  if(name.equals("maximumValue") || name.equals("minimumValue"))
		      trackEditor.getGraphicContext().getFtsSequenceObject().changeTrack(track);
    }
    
    AbstractButton b;
  }

    public TrackEditor getTrackEditor()
    {
	return trackEditor;
    }

  //--- Fields
  TrackEditor trackEditor;
  Track track;
  JToggleButton activationButton;
  JButton openButton;
  ToggleBar toggleBar;
  JPanel buttonPanel; 
  public static final int BUTTON_WIDTH = 25;
}







