
package ircam.jmax.editors.sequence;

import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.io.*;
import javax.swing.*;
import javax.swing.border.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.renderers.*;
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
    
    //activationButton = new JToggleButton(SequenceImages.getImageIcon("unselected_track"));
    trackIndex = trackEditor.getGraphicContext().getFtsSequenceObject().getTrackIndex(t);
    activationButton = new JToggleButton(""+trackIndex);
    activationButton.setMargin(new Insets(0, 0, 0, 0));
    activationButton.setPreferredSize(new Dimension(BUTTON_WIDTH, 70));

    /////////////////////////
    openButton = new JButton(SequenceImages.getImageIcon("opened_track"));
    openButton.setPreferredSize(new Dimension(BUTTON_WIDTH, 20));
    openButton.setToolTipText("open/close");

    //muteButton = new JButton();
    muteButton = new JButton(SequenceImages.getImageIcon("unmute"));
    muteButton.setPreferredSize(new Dimension(BUTTON_WIDTH, /*20*/14));
    muteButton.setToolTipText("mute/unmute");
    //muteButton.setMargin(new Insets(0, 3, 0, 3));
    //muteButton.setForeground(Color.green);
    //muteButton.setText("m");

    JPanel bp = new JPanel();
    bp.setLayout(new BoxLayout(bp, BoxLayout.Y_AXIS));
    bp.add(openButton);
    bp.add(muteButton);

    buttonPanel = new JPanel();
    buttonPanel.setLayout(new BorderLayout());
    //buttonPanel.add(openButton, BorderLayout.NORTH);
    buttonPanel.add(bp, BorderLayout.NORTH);
    buttonPanel.add(activationButton, BorderLayout.CENTER);

    add(buttonPanel, BorderLayout.WEST);

    add(trackEditor.getComponent(), BorderLayout.CENTER);

    toggleBar = new ToggleBar(trackEditor, trackIndex);
    add(toggleBar, BorderLayout.NORTH);
    toggleBar.setVisible(false);
    
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

    muteButton.addActionListener(new ActionListener(){
	public void actionPerformed(ActionEvent e)
	    {
		/*if(muteButton.isSelected() && (!mute)) 
		  track.setProperty("mute", Boolean.TRUE);
		  else 
		  if(!muteButton.isSelected() && mute) 
		  track.setProperty("mute", Boolean.FALSE);*/
		if(mute)
		    track.setProperty("mute", Boolean.FALSE);
		else
		    track.setProperty("mute", Boolean.TRUE);
	    }
	});

    // --- change the selected state of the button when the "active" property of the track changes
    track.getPropertySupport().addPropertyChangeListener( new ActiveListener(activationButton));
    track.getPropertySupport().addPropertyChangeListener( toggleBar);
    // an inner class

    validate();
  }


  /**
   * Returns the button used to activate this track.
   */
  public AbstractButton getActivationButton()
  {
    return activationButton;
  }

  public void changeIndex(int pos)
  {
      trackIndex = pos;
      activationButton.setText(""+trackIndex);
      toggleBar.changeIndex(trackIndex);
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

	      if(active) b.setForeground(Color.green);
	      else b.setForeground(Color.gray);

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
	  }
      else 
	  if(name.equals("mute"))
	  {
	      mute = ((Boolean) evt.getNewValue()).booleanValue();
	      //muteButton.setSelected(mute);
	      if(mute)
		  //muteButton.setForeground(Color.red);
		  muteButton.setIcon(SequenceImages.getImageIcon("mute"));
	      else
		  //muteButton.setForeground(Color.green);
		  muteButton.setIcon(SequenceImages.getImageIcon("unmute"));
	  }
	  else if(name.equals("maximumPitch") || name.equals("minimumPitch"))
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
      /*else 
	if(name.equals("maximumValue") || name.equals("minimumValue"))
	trackEditor.getGraphicContext().getFtsSequenceObject().changeTrack(track);*/
    }
    
    AbstractButton b;
  }

    public TrackEditor getTrackEditor()
    {
	return trackEditor;
    }

    public Track getTrack()
    {
	return track;
    }

  //--- Fields
  TrackEditor trackEditor;
  Track track;
  JToggleButton activationButton;
  JButton openButton;
  //JToggleButton muteButton;
  JButton muteButton;
  ToggleBar toggleBar;
  JPanel buttonPanel; 
  int trackIndex;
  boolean mute = false;
  public static final int BUTTON_WIDTH = 25;
}







