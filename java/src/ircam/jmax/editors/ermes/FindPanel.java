package ircam.jmax.editors.ermes;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.java.swing.*;
import com.sun.java.swing.border.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.widgets.*;

class FindPanel extends JFrame {

  public static FindPanel open()
  {
    if (findPanel == null)
      findPanel = new FindPanel();

    findPanel.setVisible(true);

    return findPanel;
  }

  public void setPatcher( FtsContainerObject patcher)
  {
    this.patcher = patcher;
  }

  protected FindPanel()
  {
    super( "Find Panel");

    JPanel labelPanel = new JPanel();

    labelPanel.setBorder( new EmptyBorder( 15, 15, 15, 15));
    labelPanel.setLayout( new BoxLayout( labelPanel, BoxLayout.X_AXIS));
    labelPanel.setOpaque( false);

    JLabel label = new JLabel("Find: ");
    label.setHorizontalTextPosition(label.RIGHT);
    label.setDisplayedMnemonic('T');
    label.setToolTipText("The labelFor and displayedMnemonic properties work!");

    textField = new JTextField( 30);

    textField.setBackground( Color.white); // ???

    label.setLabelFor( textField);
    textField.getAccessibleContext().setAccessibleName( label.getText());
    textField.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent event)
	{
	  find();
	}
    });

    labelPanel.add( label);
    labelPanel.add( textField);

    objectSetViewer = new ObjectSetViewer();

    JPanel findPanel = new JPanel();
    findPanel.setLayout( new BoxLayout( findPanel, BoxLayout.Y_AXIS));
    findPanel.setBorder( new EmptyBorder( 5, 5, 5, 5) );
    findPanel.setAlignmentX( LEFT_ALIGNMENT);
    //findPanel.setOpaque( false);

    findPanel.add( labelPanel);
    findPanel.add( objectSetViewer);

    //setSize( 300, 300);
    getContentPane().add( findPanel);

    pack();
    validate();

    set = (FtsObjectSet) Fts.newRemoteData( "objectset_data", null);
    objectSetViewer.setModel( set.getListModel());

    objectSetViewer.setObjectSelectedListener(new ObjectSelectedListener() {
      public void objectSelected(FtsObject object)
	{
	  Cursor temp = FindPanel.this.getCursor();

	  FindPanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
	  try
	    {
	      Mda.edit(object.getParent().getData(), object);
	    } 
	  catch (MaxDocumentException e)
	    {
	    }

	  FindPanel.this.setCursor(temp);
	}
    });
    
    //setBounds( 100, 100, getPreferredSize().width, getPreferredSize().height);

    // Finally, add the a tool finder to the window manager.
    // As an alternative, the find panel can be added as a window.
    // With   MaxWindowManager.getWindowManager().addWindow(this);

    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
      public String getToolName() { return "Find Panel";}
      public void open() { FindPanel.open();}
    });
  }

  public void find()
  {
    String query;
    MaxVector args;
    Cursor temp = getCursor();

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

    query = textField.getText();
    args = new MaxVector();
    FtsParse.parseAtoms(query, args);
    // set.find( patcher, args);// tmp commented
    set.find(Fts.getRootObject(), args);
    setCursor(temp);
  }

  public void findErrors()
  {
    Cursor temp = getCursor();

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
    // set.findErrors( patcher);// tmp commented
    set.findErrors(Fts.getRootObject());
    setCursor(temp);
  }

  public void findFriends(FtsObject object)
  {
    Cursor temp = getCursor();

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
    set.findFriends( object);
    setCursor(temp);
  }


  private static FindPanel findPanel = null;

  private FtsContainerObject patcher;
  private ObjectSetViewer objectSetViewer;
  private JTextField textField;
  private FtsObjectSet set;
}

