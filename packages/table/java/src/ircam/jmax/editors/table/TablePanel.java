package ircam.jmax.editors.table;

import java.lang.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.io.*;
import java.math.*;
import tcl.lang.*;
import com.sun.java.swing.*;

public class TablePanel extends JPanel implements MouseMotionListener, MouseListener{
  
  int x_scale_factor = 4;
  int N_POINTS = 128;
  int MAX_VALUE = 300;
  int old_dragx = 0;
  int old_dragy = 0;
  int values[];
  int oldValues[];
  int previousHilighted = -1;
  Tabler itsTabler;
  
  public TablePanel(Tabler theTabler) {
    super();
    itsTabler = theTabler;
    setBackground(Color.white);
    values = new int[theTabler.itsData.getSize()];
    oldValues = new int[theTabler.itsData.getSize()];
    addMouseMotionListener(this);
    addMouseListener(this);
  }

  public void initValues(int[] vector) {
    if (vector != null && vector.length != 0) {
      values = vector;
      N_POINTS = itsTabler.itsData.getSize();
      updateOldValues();
    }
  }

  public void Close() {
    setVisible(false);
  }

  public int getValue(int i) {
    if (i > 0 && i<values.length) return values[i];
    else return 0;
  }

  void updateOldValues(){
    for(int i=0;i<N_POINTS;i++){
      oldValues[i]=values[i];
    }
  }

  public void update(Graphics g) {
  }

  public void paint(Graphics g) {
    for(int i = 0; i<N_POINTS;i++){
      PaintSingle(i, g);
    }
  }
  

  public void fillTable(FtsIntegerVector aIntV) {
    if(aIntV.getSize()!=0) initValues(aIntV.getValues());
  }

   //////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////mouseMotionListener--inizio
  public void mouseMoved(MouseEvent e){
    int x = e.getX()/x_scale_factor;
    int y = e.getY();
    if (x < values.length) { 
      itsTabler.setCoordinates(x, getSize().height-y);
      //working code, commented for efficiency
      /*if (previousHilighted != -1) highlight(previousHilighted, false);
	previousHilighted = x;
	highlight(x, true);*/
    }
  }
  
  public void mouseDragged(MouseEvent e){
    int x = e.getX()/x_scale_factor;
    int y = e.getY();
    y = getSize().height-y;
    if (x<0 || x> N_POINTS-2) return;
    //oldValues[x]=values[x];
    values[x] = y;
    Interpolate(old_dragx, old_dragy, x, y);
    old_dragx = x; old_dragy = y;
    PaintSingle(x, getGraphics());
    oldValues[x]=values[x];
    //scure DoublePaint(x);
    itsTabler.setCoordinates(x, y);
  }
  //////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////mouseMotionListenr--fine

  void Interpolate(int oldx, int oldy, int newx, int newy) {
    Dimension d = size();
    Graphics temp = getGraphics();
    
    if (oldx == newx) return;	//nothing to do
    
    float factor = (newy-oldy)/Math.abs(newx-oldx);
    if (newx>oldx) for (int i=oldx+1; i<newx; i++) {
      //oldValues[i]=values[i];
      values[i] = (int) (values[i-1]+factor);
      PaintSingle(i, getGraphics());
      oldValues[i]=values[i];
      //scure DoublePaint(i);
    }
    else for (int i=oldx-1; i>newx; i--) {
      //oldValues[i]=values[i];
      values[i] = (int) (values[i+1]+factor);
      PaintSingle(i, getGraphics());
      oldValues[i]=values[i];
      //scure DoublePaint(i);
    }
  }
  
    /////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////mouseListener--inizio
  public void mouseClicked(MouseEvent e){}
  
  public void mousePressed(MouseEvent e){
    int x = e.getX()/4;
    int y = e.getY();
    y = getSize().height-y;
    if (x<0 || x>N_POINTS-2) return;
    old_dragx = x;
    old_dragy = y;
    //oldValues[x]=values[x];
    values[x] = y;
    PaintSingle(x, getGraphics());
    oldValues[x]=values[x];
    //scure DoublePaint(x);
  }

  public void mouseReleased(MouseEvent e){
    itsTabler.itsData.changed();
    itsTabler.itsData.forceUpdate();
  }

  public void mouseEntered(MouseEvent e){}
  public void mouseExited(MouseEvent e){}
  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////mouseListener--fine
  Rectangle toolRect = new Rectangle();
  void PaintSingle(int index, Graphics g) {
    Dimension d = size();
    //System.err.println("old + new "+oldValues[index]+" "+values[index]);
    if(oldValues[index]>values[index]){
      g.setColor(Color.white);
      g.fillRect(index*x_scale_factor, d.height-oldValues[index], x_scale_factor,oldValues[index]-values[index]);
    }
    else{
      g.setColor(Color.black);
      g.fillRect(index*x_scale_factor, d.height-values[index], x_scale_factor, values[index]);
    }
  }

  void highlight(int index, boolean on_off) {
    Graphics g = getGraphics();
    Dimension d = getSize();

    if (on_off) g.setColor(Color.gray);
    else g.setColor(Color.black);
    g.fillRect(index*x_scale_factor, d.height-values[index], x_scale_factor, values[index]);
  }

  void ApplyFormula(String theFormula) {
    //we should evaluate something like:
    //"set x <i>; set y values[i]; [eval <theFormula>] (or <theFormula>)
    String s = new String();
    int temp;
    int start=0;
    int end=N_POINTS-1;
    String ps = new String();
    
    //analize string s; if it begins with "[" is a subset operator
    if (theFormula.startsWith("[")) { //parserino al volo
      int p = theFormula.indexOf(']');
      int semic;
      ps = theFormula.substring(1,p);
      semic = ps.indexOf(':');
      if (semic != -1) {
	String primo = ps.substring(0,semic);
	String secondo = ps.substring(semic+1);
	start = Integer.parseInt(primo);
	end = Integer.parseInt(secondo);
      }
      else {
	start = end = Integer.parseInt(ps);
      }
      theFormula = theFormula.substring(p+1);
    }
    
    for (int i=start; i<=end; i++) {
      s = "set x " + i + "; " + "set y " + values[i] + "; " + 
	"set z " + ((i == 0) ? values[0]:values[i-1]) + "; "+
	"eval \" expr " + theFormula + "\"\n";
      try {
	MaxApplication.getTclInterp().eval(s);
	//oldValues[i]=values[i];
	values[i] = Integer.parseInt(MaxApplication.getTclInterp().getResult().toString());
      } 	catch (TclException e) {
	itsTabler.itsFormula.setText("TCL error: " + e);
	return;
      }
      catch(NumberFormatException e1) {
	itsTabler.itsFormula.setText(e1.toString());
	return;
      }
      itsTabler.itsData.changed();
    }
    paint(getGraphics());
    updateOldValues();
  }

  public Dimension preferredSize() {
    Dimension d = new Dimension();
    d.width = N_POINTS*x_scale_factor;
    d.height = MAX_VALUE;
    return d;
  }

  public Dimension minimumSize() {
    return preferredSize();
  }
}







