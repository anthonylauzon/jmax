package ircam.jmax.editors.patcher;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.widgets.*;

class FindPanel extends JFrame {

  static void registerFindPanel()
  {
    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
      public String getToolName() { return "Find Panel";}
      public void open() { FindPanel.open();}
    });
  }

  public static FindPanel open()
  {
    if (findPanel == null)
      findPanel = new FindPanel();

    findPanel.setVisible(true);

    return findPanel;
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
    findPanel.setLayout( new BorderLayout());
    findPanel.setBorder( new EmptyBorder( 5, 5, 5, 5) );
    findPanel.setAlignmentX( LEFT_ALIGNMENT);
    //findPanel.setOpaque( false);

    findPanel.add( "North", labelPanel);
    findPanel.add( "Center", objectSetViewer);

    //setSize( 300, 300);
    getContentPane().add( findPanel);

    pack();
    validate();

    set = (FtsObjectSet) Fts.newRemoteData( "object_set_data", null);
    objectSetViewer.setModel( set.getListModel());

    objectSetViewer.setObjectSelectedListener(new ObjectSelectedListener() {
      public void objectSelected(FtsObject object)
	{
	  final Cursor temp = FindPanel.this.getCursor();

	  FindPanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

	  Fts.editPropertyValue(object.getParent(), object,
				new MaxDataEditorReadyListener() {
	    public void editorReady(MaxDataEditor editor)
	      {	  FindPanel.this.setCursor(temp);}
	  });
	}
    });
    
    //setBounds( 100, 100, getPreferredSize().width, getPreferredSize().height);

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
    set.find(Fts.getRootObject(), args);
    setCursor(temp);
  }

  public void findErrors()
  {
    Cursor temp = getCursor();

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
    textField.setText("");
    set.findErrors(Fts.getRootObject());
    setCursor(temp);
  }

  public void findFriends(FtsObject object)
  {
    Cursor temp = getCursor();

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
    textField.setText("");
    set.findFriends( object);
    setCursor(temp);
  }


  private static FindPanel findPanel = null;

  private ObjectSetViewer objectSetViewer;
  private JTextField textField;
  private FtsObjectSet set;
}

