
package ircam.jmax.editors.table;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

import java.awt.*;
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.io.*;
import com.sun.java.swing.*;

/** 
 A simple table editor
 */

public class Tabler extends MaxEditor {
  
  TablePanel itsTablePanel;
  TextField itsFormula;
  JLabel itsCoordX;
  JLabel itsCoordY;
  JLabel itsCurrentValue;
  JPanel itsFrontHeader;
  MenuItem itsRefreshMenuItem;
  Dimension preferredSize = new Dimension(512,412);
  ScrollPane itsScrollPane;

  public FtsIntegerVector itsData;
  
  public Tabler(FtsIntegerVector theData) {
    super(Mda.getDocumentTypeByName("Table"));

    itsData = theData;

    if (itsData.getDocument().getName()==null)
      setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("table"));
    else {
      setTitle(itsData.getDocument().getName());
    }

    JPanel aContent = new JPanel();
    aContent.setLayout(new BorderLayout());

    aContent.setBackground(Color.white);

    //-------- front header

    itsFrontHeader = new JPanel();
    itsFrontHeader.setBackground(Color.lightGray);
    
    JPanel aPanel = new JPanel();
    aPanel.setBackground(Color.lightGray);

    JLabel aLabel1 = new JLabel("x: ");
    aLabel1.setBackground(Color.lightGray);
    JLabel aLabel2 = new JLabel("current value: ");
    aLabel2.setBackground(Color.lightGray);
    JLabel aLabel3 = new JLabel("y: ");
    aLabel3.setBackground(Color.lightGray);
    itsCoordX = new JLabel("000");
    itsCoordX.setBackground(Color.lightGray);
    itsCoordY = new JLabel("000");
    itsCoordY.setBackground(Color.lightGray);
    itsCurrentValue = new JLabel("000");
    itsCurrentValue.setBackground(Color.lightGray);

    aPanel.add(aLabel1);
    aPanel.add(itsCoordX);
    aPanel.add(aLabel2);
    aPanel.add(itsCurrentValue);
    aPanel.add(aLabel3);
    aPanel.add(itsCoordY);
    aPanel.validate();
    
    Dimension frontHeaderSize = new Dimension(512, 20);
    aPanel.setMinimumSize(frontHeaderSize);
    aPanel.setPreferredSize(frontHeaderSize);

    itsFrontHeader.add(aPanel);


    aContent.add(itsFrontHeader, BorderLayout.NORTH);
    
    //------------------ table panel into a ScrollPane

    itsTablePanel = new TablePanel(this);
    itsTablePanel.setSize(itsTablePanel.getPreferredSize().width,itsTablePanel.getPreferredSize().height);
    
    itsScrollPane = new ScrollPane();
    itsScrollPane.add(itsTablePanel);
    itsScrollPane.setScrollPosition(0, 0);
    aContent.add(itsScrollPane, BorderLayout.CENTER);

    //-----------------  formula
    itsFormula = new TextField("", 40);
    itsFormula.resize(300, 20);
    aContent.add( itsFormula, BorderLayout.SOUTH);
    
    getContentPane().add(aContent);

    Init();
    
    preferredSize.width = itsTablePanel.getPreferredSize().width;

    itsTablePanel.fillTable(itsData);
    itsTablePanel.repaint();

    itsScrollPane.setScrollPosition(0,30);
    validate();
    itsFormula.addKeyListener(this);

    setBounds(100, 100, getPreferredSize().width,getPreferredSize().height);

    setVisible(true);
  }

 
  public Tabler() {
    super();
  }

   // the MaxDataEditor interface

  public void reEdit()
  {
    if (!isVisible()) setVisible(true);
    itsData.forceUpdate();
    itsTablePanel.repaint();

    toFront();
  }

  public void SetupMenu(){
    GetEditMenu().add(new MenuItem("-"));
    itsRefreshMenuItem = new MenuItem("Refresh");
    GetEditMenu().add(itsRefreshMenuItem);
    itsRefreshMenuItem.addActionListener(new ActionListener() {
      public  void actionPerformed(ActionEvent e)
	{ 
	  itsData.forceUpdate();
	  itsTablePanel.repaint();
	}});
  }

  public void setCoordinates(int x, int y){
    if (x < 0 || x > itsData.getSize()) return;
    itsCoordX.setText(""+x);
    itsCoordY.setText(""+y);
    itsCurrentValue.setText(""+(itsTablePanel.values[x]));
  }
  
  /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio  
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}
  // Modified to use inheritance and call the MaxEditor method
  // for all the standard key bindings
  public void keyPressed(KeyEvent e){  
    int key = e.getKeyCode();
    if(key == ircam.jmax.utils.Platform.RETURN_KEY || key == ircam.jmax.utils.Platform.ENTER_KEY){
      itsFormula.setBackground(Color.gray);
      itsTablePanel.ApplyFormula(itsFormula.getText());
      itsFormula.setBackground(Color.white);
    }
    else super.keyPressed(e);
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine

  public Dimension preferredSize() {
    return /*preferredSize;*/new Dimension(512,412);
  }

  public Dimension minimumSize() {
    return preferredSize();
  }
	
}







