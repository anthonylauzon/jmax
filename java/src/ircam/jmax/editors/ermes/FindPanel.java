package ircam.jmax.editors.ermes;

import java.util.*;
import java.awt.event.*;
import com.sun.java.swing.*;
import com.sun.java.swing.border.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
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

    //setBounds( 100, 100, getPreferredSize().width, getPreferredSize().height);
  }

  public void find()
  {
    set.find( patcher, textField.getText());

    objectSetViewer.setModel( set.getListModel());
  }


  private static FindPanel findPanel = null;

  private FtsContainerObject patcher;
  private ObjectSetViewer objectSetViewer;
  private JTextField textField;
  private FtsObjectSet set;
}

