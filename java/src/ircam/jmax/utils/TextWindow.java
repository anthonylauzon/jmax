package ircam.jmax.utils;
import java.awt.*;
import java.io.*;

/**
 * The base class for all the Text window - related components.
 * Ex: TextEditor, Ermes Console.
 */
public class TextWindow extends Frame {
  int NUM_ROWS = 50;
  int NUM_COLS = 70;
  public TextArea itsTextArea;
  public PrintStream itsPrintStream;
  OutputStream itsOutputStream;
  String filler;
  String buffer = new String();

  public TextWindow() {
  }
  
  public TextWindow(String title) {
    super(title);
    setLayout(new BorderLayout());
    setBackground(Color.white);
    setForeground(Color.blue);
    Font thisFont = new Font(Platform.FONT_NAME, Font.PLAIN, Platform.FONT_SIZE);
    setFont(thisFont);
    filler = new String();
    for (int i = 0; i< NUM_COLS; i++) {
      filler += "_";
    }
    itsOutputStream = new TWOutputStream(this);
    itsPrintStream = new PrintStream(itsOutputStream);
    buffer = "";
    Init();//???
  }
  
  public boolean handleEvent(Event event) {
    if (event.id == Event.WINDOW_DESTROY) {
      dispose();
    }
    return super.handleEvent(event);
  }
 
  public boolean keyDown(Event e, int k){

    if(k == Platform.DELETE_KEY || k == Platform.BACKSPACE_KEY) {
      if (buffer.length() != 0) buffer = buffer.substring(0,buffer.length()-1);
    }
    else if (k == Platform.NEWLINECHAR) {
      if (buffer != "") {
	buffer = "";
      }
    }
    else buffer += (char) k;
    return false;
  }
  
  public PrintStream getPrintStream() {
    return itsPrintStream;
  }
  
  public void Init() {
    Rectangle r = bounds();
    
    itsTextArea = new TextArea(NUM_ROWS, NUM_COLS);
    itsTextArea.setEditable(true);
    itsTextArea.setBackground(Color.white);
    add("Center", itsTextArea);
    validate();
    itsTextArea.show();
    itsTextArea.requestFocus();
  }
  
  public Dimension preferredSize() {
    Dimension d = new Dimension();
    d.width = itsTextArea.getFontMetrics(itsTextArea.getFont()).stringWidth(filler);
    d.height = itsTextArea.getFontMetrics(itsTextArea.getFont()).getHeight() * NUM_ROWS;
    return d;
  }
  
  public Dimension minimumSize() {
    return preferredSize();
  }
}






