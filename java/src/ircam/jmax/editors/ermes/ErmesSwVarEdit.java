package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;
import java.awt.event.*;

import ircam.jmax.*;
import com.sun.java.swing.*;
import com.sun.java.swing.plaf.*;
import com.sun.java.swing.border.*;

public class ErmesSwVarEdit extends JPanel implements ActionListener{
  ErmesSketchPad itsSketchPad;
  JPanel betweex;
  JComboBox itsCombo;
  int itsNRow = 0;
  int preferredHeight = 100;// it will change with split panes
  //---------------------

  public void actionPerformed(ActionEvent e) {
    addALine();
    Dimension pre_min = new Dimension(400, 30*itsNRow); 
    betweex.setPreferredSize(pre_min);
    betweex.setMinimumSize(pre_min);
    betweex.setSize(pre_min);
    //1. invalidate();
    validate();
    repaint();
    //validate();
  }
  //-----------------------
  public ErmesSwVarEdit(ErmesSketchPad theSketchPad, int theNRow) {

    itsSketchPad = theSketchPad;
    setLayout(new ErmesToolBarLayout(ErmesToolBarLayout.VERTICAL));

    setDoubleBuffered(/*3.true*/false);
    JButton addButton = new JButton("Add");
    addButton.addActionListener(theSketchPad.GetSketchWindow());
    JButton deleteButton = new JButton("Delete");
    JButton editButton = new JButton("Edit");
    JToolBar aToolBar = new JToolBar();
    aToolBar.add(addButton);
    addButton.addActionListener(this);
    aToolBar.add(deleteButton);
    aToolBar.setSize(300, 30);
    aToolBar.setFloatable(false);   
    add(aToolBar, "stick_both");
    validate();
    //-----second element; the "betweex panel"
    betweex = new JPanel();
    //betweex.setLayout(new GridLayout(0,1));
    betweex.setLayout(new ErmesToolBarLayout(ErmesToolBarLayout.VERTICAL, 3));
    for (int i=0; i< theNRow; i++) {
      addALine();
    }
    Dimension pre_min = new Dimension(400, 30*itsNRow); 
    betweex.setPreferredSize(pre_min);
    betweex.setMinimumSize(pre_min);
    betweex.setSize(pre_min);

    ScrollPane aScrollPane = new ScrollPane(ScrollPane.SCROLLBARS_ALWAYS);
    ///***questo era il vero, non la sua copia ||aScrollPane.add(betweex, 0);
    ///aScrollPane.setSize(betweex.getSize().width, betweex.getSize().height);
    aScrollPane.setSize(betweex.getSize().width, preferredHeight);///***absolutely remove
    aScrollPane.add(betweex, 0);
    add(aScrollPane, "stick_both");
    validate();
    //setSize(400, 180);// creation time, fixed lenght///*** era 400, 60
    setSize(getPreferredSize());
  }
  
  
  void addALine() {
    ErmesVarEditRow aEditRow = new ErmesVarEditRow(this);
    itsNRow+=1;
    //from the Swing short course: did'nt work
    // aEditRow.setBackground (Color.lightGray);
    aEditRow.setBorder (new BevelBorder(BevelBorder.RAISED));
    //content.add (b);

    betweex.add(aEditRow, "stick_both");
    betweex.validate();
  } 


  // every line is:
    //(1) a combo
    //(2) two edit fields
  class ErmesVarEditRow extends JPanel {
    ErmesSwVarEdit itsVarEdit;

    public ErmesVarEditRow(ErmesSwVarEdit theVarEdit){
    super();
    itsVarEdit = theVarEdit;
    setLayout(new ErmesToolBarLayout(ErmesToolBarLayout.HORIZONTAL, 1));
    //(1) two edit fields
    Label aLabel1 = new Label("Name:");
    //---------- -30
    aLabel1.setSize(40+30, 30);
    TextField varName = new TextField("variable name", 10);
    varName.setSize(60+30, 30);
    Label aLabel2 = new Label("Value:");
    aLabel2.setSize(50+30, 30);
    TextField defaultValue = new TextField("value", 10);
    defaultValue.setSize(30+30, 30);
    
    add(aLabel1);
    add(varName);
    add(aLabel2);
    add(defaultValue);
    //(2) a combobox
    Label aLabel3 = new Label("Type:");
    aLabel3.setSize(40+30, 30);
    JComboBox varType = new JComboBox();
    varType.addItem("int");
    varType.addItem("float");
    varType.addItem("symbol");
    varType.setEditable(true);
    varType.setSelectedItem("type");
    varType.setMaximumRowCount(4);
    varType.setSize(60+30, 30);
    add(aLabel3);
    varType.addActionListener(itsVarEdit);
    //    varType.addMouseListener(itsVarEdit);
    itsVarEdit.itsCombo = varType;
    add(varType);
    validate();
    //setSize(400, 30); 
    }
    
    public Dimension getPreferredSize() {
    return new Dimension(40+60+50+30+40+60, 30+4);
    }
  }

  ///*** non c'era
  public Dimension getPreferredSize() {
    return new Dimension(400, preferredHeight); 
  }
}

                    

