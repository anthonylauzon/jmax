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

package ircam.jmax.editors.qlist;

import ircam.jmax.*;
import ircam.jmax.fts.*;

import java.awt.*;
import java.awt.datatransfer.*;
import java.awt.event.*;
import java.io.*;

import javax.swing.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.dialogs.*;

import ircam.jmax.editors.qlist.actions.*;
import ircam.jmax.ispw.*;

/**
 * A panel that is able to show the content of a FtsAtomList (qlist).
 * This component does not handle the communication with FTS, but it offers
 * a simple API (fillContent, getText) in order to be used from outside. */
public class QListPanel extends JPanel implements Editor, ClipboardOwner
{  
  JTextArea itsTextArea;
  int caretPosition;
  Dimension preferred = new Dimension(512, 412);
  QListFindDialog qListFindDialog;

  String textToFind;
  int lastFindIndex;

  FtsQListObject itsData;
  boolean changed = false;

  /**
   * Constructor */
  public QListPanel(EditorContainer container, FtsQListObject theContent) 
  { 
    super();

    itsEditorContainer = container;
    itsData = theContent;

    // prepare the Set & Get buttons panel
    JPanel aPanel = new JPanel();
    aPanel.setLayout(new GridLayout(1, 2));
    
    JButton setButton = new JButton("Set");
    setButton.addActionListener(Actions.setAction);

    JButton getButton = new JButton("Get");
    getButton.addActionListener(Actions.getAction);

    aPanel.add(setButton);
    aPanel.add(getButton);
    aPanel.validate();
    ///////////////////////////////////////////

    itsTextArea = new JTextArea(40, 40);
    itsTextArea.addKeyListener(KeyConsumer.controlConsumer());
    // SGI's JTextArea bug. See utils.KeyConsumer class for details
    itsTextArea.addKeyListener(new KeyListener(){
	    public void keyTyped(KeyEvent e){}
	    public void keyPressed(KeyEvent e){ changed = true;}
	    public void keyReleased(KeyEvent e){}	    
	});

    setLayout(new BorderLayout());
    add(BorderLayout.NORTH, aPanel);
    add(BorderLayout.CENTER, new JScrollPane(itsTextArea));
    
    caretPosition = 0;
    validate();

    textToFind = "";
    lastFindIndex = 0;

    // prepare the find dialog 
    qListFindDialog = new QListFindDialog(itsEditorContainer.getFrame(), this);

    fillContent(theContent);
    
    theContent.getAtomList().addFtsAtomListListener(new FtsAtomListListener(){
	public void contentChanged(){
	  fillContent(itsData);
	}
      });
  }

  /**
   * Sets the content to the given FtsAtomList object */
  public void fillContent(FtsQListObject theContent) 
  {
    String text = FtsMessageObject.preParseMessage( QListUnparse.unparseDescription( theContent.getAtomList().getValues()));

    if( ! text.equals( itsTextArea.getText())){
      caretPosition = itsTextArea.getCaretPosition();
      itsTextArea.setText( text);
      itsTextArea.requestFocus();
      // (em) added a control to avoid setting impossible caret positions.
      // FtsAtomList.getValueAsText() can infact reformat the text,
      // removing CR's and then shortening its length
      if (caretPosition <= itsTextArea.getText().length())
	itsTextArea.setCaretPosition( caretPosition);
      else itsTextArea.setCaretPosition(itsTextArea.getText().length());
    }
    changed = false;
  }
 
  public Dimension getPreferredSize() 
  {
    return preferred;
  }

  public Dimension getMinimumSize() 
  {
    return preferred;
  }

  /**
   * part of API offered to the findDialog */
  int getCaretPosition()
  {
    return itsTextArea.getCaretPosition();
  }

  /**
   * returns the text containing in this panel. */
  public String getText()
  {
    return itsTextArea.getText();
  }

  public JTextArea getTextArea(){
    return itsTextArea;
  }

  public boolean isSelectedText(){
    String text = itsTextArea.getSelectedText();
    if(text==null) return false;
    if(text.equals("")) return false;
    else return true;
  }

  /**
   * part of API offered to the findDialog */
  int find( String textToFind, int fromIndex)
  {
    int index = itsTextArea.getText().indexOf( textToFind, fromIndex);

    if ( index >= 0)
      {
	itsTextArea.select( index, index + textToFind.length());
      }

    return index;
  }
  /////////////////////////////////////////
  public void Cut(){
    if(itsTextArea.getSelectedText() != null){
      Copy();
      String s = itsTextArea.getText();
   
      itsTextArea.setText(s.substring(0, itsTextArea.getSelectionStart()) +  s.substring(itsTextArea.getSelectionEnd(), s.length()));
    }
  }

  public void Copy(){
    if (itsTextArea.getSelectedText() != null){
      String toCopy = itsTextArea.getSelectedText();
      JMaxApplication.getSystemClipboard().setContents(new StringSelection(toCopy), this);
    }
  }

  public void Paste(){
    Transferable clipboardContent = JMaxApplication.getSystemClipboard().getContents(this);
    if (clipboardContent.isDataFlavorSupported(DataFlavor.stringFlavor)){
      try{
	String toPaste = (String) clipboardContent.getTransferData(DataFlavor.stringFlavor);
	
	itsTextArea.insert(toPaste, itsTextArea.getCaretPosition());
	itsTextArea.requestFocus();
	
      } catch (Exception e) { System.err.println("error in paste: "+e);}
    }
  }

    public void Get(){
      if(changed)
	{
	  Object[] options = { "Discard changes", "Cancel"};
	  int result = JOptionPane.showOptionDialog(itsEditorContainer.getFrame(),
						    "Do you want really discard changes in QList text?",
						    "Warning",
						    JOptionPane.YES_NO_OPTION,
						    JOptionPane.QUESTION_MESSAGE,
						    null, options, options[0]);  

	  if(result == JOptionPane.YES_OPTION)
	    {
	      itsData.getAtomList().forceUpdate();
	      fillContent(itsData);
	    }
	}
      else
	{
	  itsData.getAtomList().forceUpdate();
	  fillContent(itsData);
	}
    }

  public void Set(){
    itsData.getAtomList().setValuesAsText(getText());
  }

  public void Find(){
    qListFindDialog.open();
  }
  public void FindAgain(){
    qListFindDialog.find();
  }
  public void Import(){
    File file = MaxFileChooser.chooseFileToOpen(itsEditorContainer.getFrame(), "Import");

    if (file != null)
      {
	try
	  {
	    StringBuffer buf = new StringBuffer();
	    BufferedReader in = new BufferedReader(new FileReader(file));

	    while (in.ready())
	      {
		buf.append(in.readLine());
		buf.append("\n");
	      }

	    itsTextArea.setText(buf.toString());
	    itsData.getAtomList().setValuesAsText(itsTextArea.getText());
	  }
	catch (java.io.IOException e)
	  {
	      JOptionPane.showMessageDialog(itsEditorContainer.getFrame(), 
					    "Cannot open qlist "+e, 
					    "Error", JOptionPane.ERROR_MESSAGE); 
	  }
      }
  }

  public void Export()
  {
    File file;
    Writer w;

    file = MaxFileChooser.chooseFileToSave(itsEditorContainer.getFrame(), new File("qlist.txt"),  "Export");

    if (file == null)
      return;

    try
      {
	w = new FileWriter(file);

	w.write(itsTextArea.getText());
	w.close();
      }
    catch (java.io.IOException e)
      {
	  JOptionPane.showMessageDialog(itsEditorContainer.getFrame(), 
					"Cannot save qlist: "+e, 
					"Error", JOptionPane.ERROR_MESSAGE); 
      }
  }
  //------------------- Editor interface ---------------
  EditorContainer itsEditorContainer;

  public EditorContainer getEditorContainer(){
    return itsEditorContainer;
  }

  public void Close(boolean doCancel){
    ((Component)itsEditorContainer).setVisible(false);
    itsData.requestDestroyEditor();
    MaxWindowManager.getWindowManager().removeWindow((Frame)itsEditorContainer);
  }
  // ----------ClipboardOwner interface methods
  public void lostOwnership(Clipboard clipboard, Transferable contents) {}
}



