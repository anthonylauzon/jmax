//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

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

// ^^^^ Same thing as for the control panel; the find panel itself is
// ^^^^ ready to be used with a specific server, but it is not clear
// ^^^^ how to make the binding; static, dynamic, in parallel on all the server ?
// ^^^^ The is a big user environment question to be solved before the technical one.


public class FindPanel extends JFrame
{
  Fts fts;

  static void registerFindPanel()
  {
    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
      public String getToolName() { return "Find Panel";}
      public void open() { FindPanel.open(MaxApplication.getFts());}
    });
  }

  public static FindPanel open(Fts fts)
  {
    if (findPanel == null)
      findPanel = new FindPanel(fts);

    findPanel.setVisible(true);

    return findPanel;
  }

  public static FindPanel getInstance(Fts fts)
  {
    if (findPanel == null)
      findPanel = new FindPanel(fts);

    return findPanel;
  }

  protected FindPanel(Fts f)
  {
    super( "Find Panel");

    this.fts = f;
    
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

    set = (FtsObjectSet) fts.newRemoteData( "object_set_data", null);
    objectSetViewer.setModel( set.getListModel());

    objectSetViewer.setObjectSelectedListener(new ObjectSelectedListener() {
      public void objectSelected(FtsObject object)
	{
	  final Cursor temp = FindPanel.this.getCursor();

	  FindPanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

	  fts.editPropertyValue(object.getParent(), object,
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
    set.find(fts.getRootObject(), args);
    setCursor(temp);
  }

  public void findErrors()
  {
    Cursor temp = getCursor();

    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
    textField.setText("<find errors>");
    set.findErrors(fts.getRootObject());
    setCursor(temp);
  }

  public void findFriends(FtsObject object)
  {
    Cursor temp = getCursor();
    setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));
    textField.setText("");

    //waiting for a bug fix (findfriends doasen't work
    set.find(fts.getRootObject(), object.getClassName());
    //set.findFriends( object);

    setCursor(temp);
  }


  private static FindPanel findPanel = null;

  private ObjectSetViewer objectSetViewer;
  private JTextField textField;
  private FtsObjectSet set;
}

