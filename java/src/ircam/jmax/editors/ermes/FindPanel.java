package ircam.jmax.editors.ermes;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.java.swing.*;
import com.sun.java.swing.border.*;

import ircam.jmax.*;
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

    JPanel p1 = new JPanel();
    p1.setBorder( new EmptyBorder(5,5,5,5));
    p1.setLayout( new BoxLayout( p1, BoxLayout.X_AXIS));

    JLabel label = new JLabel("Find: ");
    label.setHorizontalTextPosition(label.RIGHT);

    textField = new JTextField("");
    label.setLabelFor( textField);
    textField.getAccessibleContext().setAccessibleName( label.getText());
    textField.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent event)
	{
	  find();
	}
    });

    p1.add( label);
    p1.add( textField);

    objectSetViewer = new ObjectSetViewer();

//     JPanel p2 = new JPanel();
//     p2.setLayout( new BoxLayout( p2, BoxLayout.Y_AXIS));

//     p2.add( objectSetViewer);

    JPanel findPanel = new JPanel();
    findPanel.setBorder( new EmptyBorder( 5, 5, 5, 5) );
    findPanel.setLayout( new BoxLayout( findPanel, BoxLayout.Y_AXIS));
    findPanel.setAlignmentX(LEFT_ALIGNMENT);

    findPanel.add( p1);
    //    findPanel.add( p2);
    findPanel.add( objectSetViewer);

    getContentPane().add( findPanel);

    pack();
    validate();

    set = (FtsObjectSet) Fts.newRemoteData( "objectset_data", null);

    objectSetViewer.setObjectSelectedListener(new ObjectSelectedListener() {
      public void objectSelected(FtsObject object)
	{
	  try
	    {
	      Mda.edit(object.getParent().getData(), object);
	    } 
	  catch (MaxDocumentException e)
	    {
	    }
	}
    });
    
    //setBounds( 100, 100, getPreferredSize().width, getPreferredSize().height);

    // Finally, add the panel to the window manager, so it will appear
    // in the Window menu (Should it ?????) MDC.

    MaxWindowManager.getWindowManager().addWindow(this);
  }

  public void find()
  {
    String query;
    Vector args;
    Cursor temp = getCursor();

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

    query = textField.getText();
    args = new Vector();
    FtsParse.parseAtoms(query, args);

    objectSetViewer.setModel( null);
    set.find( patcher, args);
    objectSetViewer.setModel( set.getListModel());

    setCursor(temp);
  }

  public void findErrors()
  {
    Cursor temp = getCursor();

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
    objectSetViewer.setModel( null);
    set.findErrors( patcher);
    objectSetViewer.setModel( set.getListModel());

    setCursor(temp);
  }

  public void findFriends(FtsObject object)
  {
    Cursor temp = getCursor();

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
    objectSetViewer.setModel( null);
    set.findFriends( object);
    objectSetViewer.setModel( set.getListModel());

    setCursor(temp);
  }


  private static FindPanel findPanel = null;

  private FtsContainerObject patcher;
  private ObjectSetViewer objectSetViewer;
  private JTextField textField;
  private FtsObjectSet set;
}

