//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.qlist;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

class QListFindDialog extends Dialog {
  public QListFindDialog( QList qList, QListPanel qListPanel)
  {
    super( (Frame)qList, "Qlist: Find", false);

    this.qListPanel = qListPanel;

    continueDialog = new QListContinueDialog( (Frame)qList, this);

    Panel p1 = new Panel();
    p1.setLayout( new FlowLayout( FlowLayout.LEFT));
   
    p1.add(new Label("Find:"));

    textField = new TextField( "", 40);
    textField.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e) 
	{
	  find();
	}
    } );
    p1.add(textField);
    
    add( "North", p1);

    Panel p2 = new Panel();
    p2.setLayout( new BorderLayout());
    
    Button findButton = new Button("Find");
    findButton.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e) 
	{
	  find();
	}
    } );
    p2.add( "East", findButton);

    Button closeButton = new Button("Close");
    closeButton.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e) 
	{
	  close(); 
	}
    } );
    p2.add( "West", closeButton);
    
    add("South", p2);

    pack();
  }

  class QListContinueDialog extends Dialog {
    public QListContinueDialog( Frame parent, QListFindDialog asker)
    {
      super( parent, "Qlist: Question", false);

      this._asker = asker;

      Panel p1 = new Panel();
      p1.setLayout( new FlowLayout( FlowLayout.LEFT));
   
      p1.add( new Label( "End reached; continue from beginning?" ) );

      add( "North", p1);

      Panel p2 = new Panel();
    
      Button okButton = new Button( "OK");
      okButton.addActionListener( new ActionListener() {
	public void actionPerformed( ActionEvent e)
	  {
	    setVisible( false);
	    _asker.restart();
	  }
      } );
      p2.add( okButton);

      Button cancelButton = new Button( "Cancel");
      cancelButton.addActionListener( new ActionListener() {
	public void actionPerformed( ActionEvent e)
	  {
	    setVisible( false);
	  }
      } );
      p2.add( cancelButton);
    
      add( "South", p2);

      pack();
    }

    protected QListFindDialog _asker;
  }

  protected void doFind( int fromIndex)
  {
    int index;

    index = qListPanel.find( textField.getText(), fromIndex);

    if ( index < 0)
      continueDialog.setVisible( true);
  }

  protected void restart()
  {
    doFind( 0);
  }

  public void find()
  {
    doFind( qListPanel.getCaretPosition());
  }

  public void open()
  {
    setVisible( true);
  }

  public void close()
  {
    setVisible( false);
  }

  TextField textField;
  QListContinueDialog continueDialog;
  QListPanel qListPanel;
}
