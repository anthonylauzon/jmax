package ircam.jmax.editors.explode;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;

/**
 * basic inspector for a generic adapter.
 * It allows the editing of the mappings between score parameters
 * and graphic parameters.
 * It makes (for now) the assumption on the names of the graphic 
 * parameters to be mapped. The client is called back when a mapping
 * changes, with the two names associated to the new mapping (ex. "lenght" to "time")
 * call it statically via the createSettingsDialog call.
 */
class SettingsDialog extends JDialog {
  

    /**
     * an inner utility class to represent a single map:
     * it builds a panel with the given (graphic) name associated to
     * each score parameter in a ButtonGroup.
     */
    class SingleMapping extends JPanel {
      
      public SingleMapping(String name) 
      {
	
	setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
	
	itsParamName = name;

	TitledBorder mappingTitle = new TitledBorder(LineBorder.createGrayLineBorder(), "Mappings: "+itsParamName);
	mappingTitle.setTitleFont(titleFont);
	setBorder(mappingTitle);
    
	JRadioButton pitch = new JRadioButton("Note");
	pitch.addActionListener(new ActionListener() 
				{
				  public void actionPerformed(ActionEvent e) 
				    {
				      notifyListener(itsParamName, "pitch");
				    }
				});
	JRadioButton duration = new JRadioButton("Dur.");
	duration.addActionListener(new ActionListener() 
				{
				  public void actionPerformed(ActionEvent e) 
				    {
				      notifyListener(itsParamName, "duration");
				    }
				});
	JRadioButton velocity = new JRadioButton("Vel.");
	velocity.addActionListener(new ActionListener() 
				{
				  public void actionPerformed(ActionEvent e) 
				    {
				      notifyListener(itsParamName, "velocity");
				    }
				});
	JRadioButton channel = new JRadioButton("Ch.");
    	channel.addActionListener(new ActionListener() 
				{
				  public void actionPerformed(ActionEvent e) 
				    {
				      notifyListener(itsParamName, "channel");
				    }
				});

	ButtonGroup aButtonGroup = new ButtonGroup();
	aButtonGroup.add(pitch);
	aButtonGroup.add(duration);
	aButtonGroup.add(velocity);
	aButtonGroup.add(channel);
    
	add(pitch);
	add(duration);
	add(velocity);
	add(channel);
      }

      public void setMappingListener(MappingListener theListener) 
      {
	itsMappingListener = theListener;
      }

      public void notifyListener(String graphicParam, String scoreParam)
      {
	itsMappingListener.mappingChanged(graphicParam, scoreParam);
      }

      //--- Fields
      MappingListener itsMappingListener;
      String itsParamName;
    }



  /**
   * builds a setting dialog for the given adapter, using the given frame
   * as "dialog parent" frame
   */
  public SettingsDialog (Adapter theAdapter, Frame theFrame) {
    super(theFrame, theAdapter.getName()+"Settings");

    itsAdapter = theAdapter;

    Box box = new Box(BoxLayout.Y_AXIS);
 
    titleFont = new Font(theFrame.getFont().getName(), Font.BOLD, theFrame.getFont().getSize());
    
    SingleMapping yMap = new SingleMapping("y");
    yMap.setMappingListener(itsAdapter);
    SingleMapping lMap = new SingleMapping("lenght");
    lMap.setMappingListener(itsAdapter);
    SingleMapping sMap = new SingleMapping("label");
    sMap.setMappingListener(itsAdapter);
    
    JPanel buttons = new JPanel();
    JButton applyButton = new JButton("Apply");
    applyButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	//System.err.println(".");
      }
    });

    JButton cancelButton = new JButton("Cancel");
    cancelButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	setVisible(false);
	dispose();
      }
    });

    buttons.add(cancelButton);
    buttons.add(applyButton);
    
    box.add(yMap);
    box.add(lMap);
    box.add(sMap);
    box.add(buttons);

    // finally...
    getContentPane().add(box);

  }


  /**
   * static constructor. Use this function to create the dialog
   */
  public static SettingsDialog createSettingsDialog(Adapter theAdapter, Frame theFrame) 
  {
    return new SettingsDialog(theAdapter, theFrame);
  }

  //---- Fields
  Font titleFont;
  Adapter itsAdapter;
}


