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

    JPanel panel = new JPanel();
    panel.setBorder( new TitledBorder( "Floating-point errors"));
    panel.setLayout( new BorderLayout());

    JPanel p1 = new JPanel();
    p1.setBorder( new EmptyBorder( 3, 0, 3, 0));
    p1.setLayout( new BoxLayout( p1, BoxLayout.Y_AXIS));

    JCheckBox nanCheckBox = new JCheckBox( "Check NaN");
    nanCheckBox.addItemListener( new ItemListener() {
      public void itemStateChanged( ItemEvent e)
	{
	    if (e.getStateChange() == ItemEvent.DESELECTED)
	      Fts.getDspController().setCheckNan( false);
	    else  if (e.getStateChange() == ItemEvent.SELECTED)
	      Fts.getDspController().setCheckNan( true);
	}
    });
    p1.add( nanCheckBox);

    panel.add( p1, "North");

    objectSetViewer = new ObjectSetViewer();

    panel.add( objectSetViewer, "Center");

    JPanel p3 = new JPanel();
    p3.setBorder( new EmptyBorder( 3, 0, 3, 0));

    p3.setLayout( new BoxLayout( p3, BoxLayout.X_AXIS));

    JButton clearButton = new JButton("Clear");
    clearButton.setMargin( new Insets(2,2,2,2));
    clearButton.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e)
	{
	  Fts.getDspController().clearFpeCollecting();
	}});

    p3.add( clearButton);

    p3.add( Box.createGlue());

    JButton closeButton = new JButton( "Close");
    closeButton.setMargin( new Insets(2,2,2,2));
    closeButton.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e)
	{
	  setVisible( false);
	}
    });
    p3.add( closeButton);

    panel.add( p3, "South");

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

