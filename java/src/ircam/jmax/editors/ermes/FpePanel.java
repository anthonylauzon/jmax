//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.ermes;

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

class FpePanel extends JFrame implements WindowListener
{
  private static FpePanel fpePanel = null;
  private ObjectSetViewer objectSetViewer;
  private FtsObjectSet set;

  static void registerFpePanel()
  {
    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
      public String getToolName() { return "Fpe Panel";}
      public void open() { FpePanel.open();}
    });
  }

  public static FpePanel open()
  {
    if (fpePanel == null)
      fpePanel = new FpePanel();

    fpePanel.setVisible(true);

    return fpePanel;
  }

  protected FpePanel()
  {
    super( "Fpe Panel");

    addWindowListener(this);

    JLabel label = new JLabel("Objects With Floating Point Exceptions ");
    label.setHorizontalTextPosition(label.RIGHT);

    JPanel labelPanel = new JPanel();
    labelPanel.setLayout( new BorderLayout());
    labelPanel.setBorder( new EmptyBorder( 1, 1, 1, 1) );
    labelPanel.add("West", label);

    JButton clearButton = new JButton("Clear");
    clearButton.setMargin( new Insets(2,2,2,2));
    clearButton.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e)
	{
	  Fts.getDspController().clearFpeCollecting();
	}});

    labelPanel.add("East", clearButton);

    objectSetViewer = new ObjectSetViewer();

    JPanel panel = new JPanel();
    panel.setLayout( new BorderLayout());
    panel.setBorder( new EmptyBorder( 5, 5, 5, 5) );
    panel.setAlignmentX( LEFT_ALIGNMENT);
    //FpePanel.setOpaque( false);

    panel.add( "North", labelPanel);
    panel.add( "Center", objectSetViewer);

    //setSize( 300, 300);
    getContentPane().add( panel);

    pack();
    validate();

    set = (FtsObjectSet) Fts.newRemoteData( "object_set_data", null);
    objectSetViewer.setModel( set.getListModel());

    Fts.getDspController().startFpeCollecting(set);

    objectSetViewer.setObjectSelectedListener(new ObjectSelectedListener() {
      public void objectSelected(FtsObject object)
	{
	  final Cursor temp = FpePanel.this.getCursor();

	  FpePanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

	  Fts.editPropertyValue(object.getParent(), object,
				new MaxDataEditorReadyListener() {
	    public void editorReady(MaxDataEditor editor)
	      {	  FpePanel.this.setCursor(temp);}
	  });
	}
    });
    
    //setBounds( 100, 100, getPreferredSize().width, getPreferredSize().height);
  }

  public void windowClosing(WindowEvent e)
  {
  }

  public void windowOpened(WindowEvent e)
  {
    Fts.getDspController().startFpeCollecting(set);
  }

  public void windowClosed(WindowEvent e)
  {
    Fts.getDspController().stopFpeCollecting();
  }

  public void windowIconified(WindowEvent e)
  {
    Fts.getDspController().stopFpeCollecting();
  }

  public void windowDeiconified(WindowEvent e)
  {
    Fts.getDspController().startFpeCollecting(set);
  }

  public void windowActivated(WindowEvent e)
  {
  }

  public void windowDeactivated(WindowEvent e)
  {
  }
}




