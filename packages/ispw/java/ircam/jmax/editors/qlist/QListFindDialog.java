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

package ircam.jmax.editors.qlist;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

class QListFindDialog extends Dialog {
  public QListFindDialog(Frame frame, QListPanel qListPanel)
  {
    super(frame, "Qlist: Find", false);

    this.qListPanel = qListPanel;

    continueDialog = new QListContinueDialog(frame, this);

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
