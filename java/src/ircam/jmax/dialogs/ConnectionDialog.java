
package ircam.jmax.dialogs;

import ircam.jmax.*;
import java.awt.*;
import java.awt.event.*;
import ircam.jmax.utils.*;

//	 _____________________________________________
//	|		ConnectionDialog	(Border)	  			  |
//	|	 _______________________  	  			  |
//	| 	|	North  = Label		| 	  			  |
//	|  	 _______________________  	  			  |
//	|	 ___________________________________	  |
//	| 	| 	West = p1	(Border)			| 	  |
//	| 	|  _____________________________	| 	  |
//	| 	| | p11 = North  (Flow)			|	| 	  |
//	| 	| |  label,  choice			 	|	| 	  |
//	| 	|  _____________________________	| 	  |
//	| 	|  _____________________________	| 	  |
//	| 	| | p12 = West  (Border)		|	| 	  |
//	| 	| |  _________________________	|	| 	  |
//	| 	| | |p121 = North  (Flow)	  |	|	| 	  |
//	|	| | | label, TextField		  | |	|	  |
//	|	| |  _________________________	|	|	  |
//	| 	| |  _________________________	|	| 	  |
//	| 	| | |p122 = West  (Flow)	  |	|	| 	  |
//	|	| | | label, TextField		  | |	|	  |
//	|	| |  _________________________	|	|	  |
//	|	|  _____________________________	|	  |
//	|	 ___________________________________	  |
//	|	 ___________________________________	  |
//	| 	| 	South = p2	(Border)			| 	  |
//	|	| Ok (West) , cancel(east)			|	  |
//	|	 ___________________________________	  |
//	 _____________________________________________

/**
 * The dialog popped up at startup to establish the connection with FTS.
 * Use it thru the static function "PopUpConnectionDialog".
 */

public class ConnectionDialog extends Dialog implements KeyListener, ActionListener, ItemListener{
  Button okButton;
  Button cancelButton;
  Choice connectionType;
  TextField host, port;
  public final static int NO_CONNECTION = 0;
  public final static int LOCAL_CONNECTION = 1;
  public final static int REMOTE_CONNECTION = 2;
    
  public int connectionLine = NO_CONNECTION;
  public String hostName = "";
  public String portNo = "";
    
  public ConnectionDialog() {
    super(MaxWindowManager.getWindowManager().getAFrame(), "FTS connection", false);

    setLayout(new BorderLayout());
		
    //Create north section.
    add("North",new Label("      Choose an FTS connection:"));

    //Create Center section.
    Panel p1 = new Panel();
    p1.setLayout(new BorderLayout());

    //	CHOICE CONTROL //
    Panel p11 = new Panel();        
    p11.setLayout(new FlowLayout(FlowLayout.LEFT));
		        
    p11.add(new Label("connection type"));
    connectionType = new Choice();        
    connectionType.addItem("none");
    connectionType.addItem("local");
    connectionType.addItem("remote");
    connectionType.addItemListener(this);
    p11.add(connectionType);
    p1.add("North", p11);
			
    //	host and port CONTROLS //
    Panel p12 = new Panel();
    p12.setLayout(new BorderLayout());
				
    Panel p121 = new Panel();
    p121.setLayout(new FlowLayout(FlowLayout.LEFT));
					
    p121.add(new Label("Host name"));
    host = new TextField("", 20);
    host.addActionListener(this);
    p121.add(host);
    p12.add("North",p121);
				
    Panel p122 = new Panel();
    p122.setLayout(new FlowLayout(FlowLayout.LEFT));
    p122.add(new Label("Port num."));
    port = new TextField("2000");
    port.addActionListener(this);
    p122.add(port);
		        
    p12.add("West", p122);        
    p1.add("West",p12);	////        

    add("West", p1);	////        

    //Create south section.
    Panel p2 = new Panel();
    p2.setLayout(new BorderLayout());
	        
    okButton = new Button("OK");
    okButton.setBackground(Color.white);
    okButton.addActionListener(this);
    p2.add("East", okButton);
    cancelButton = new Button("Cancel");
    cancelButton.setBackground(Color.white);
    cancelButton.addActionListener(this);
    p2.add("West", cancelButton);
    add("South", p2);

    addKeyListener(this);

    //Initialize this dialog to its preferred size.

    pack();

    // set to a hardcoded location

    setLocation(200,200);
    setVisible(true);
  }

  private void  connectionChoosen()
  {
    String connType;

    if (connectionLine == REMOTE_CONNECTION) 
      connType = "socket";
    else if (connectionLine == LOCAL_CONNECTION)
      connType = "local";
    else return;

    // Work only in astor and mimi, the directory is fake !!
    // also the whole tcl init structure would not work; it must
    // be called from the TCL script, and not here !!!

    ircam.jmax.fts.FtsServer.connectToFts("/usr/local/max/fts/bin/origin/opt" , "fts", connType, hostName, portNo);
  }

  ////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////actionListener--inizio

  public void actionPerformed(ActionEvent e){    
    if (e.getSource() == okButton){
      hostName = host.getText();
      portNo = port.getText();
      connectionChoosen();
      setVisible(false);
      dispose();
    }
    else if (e.getSource() == cancelButton) setVisible(false);
    //else if(e.getSource() == connectionType) connectionLine = connectionType.getSelectedIndex();
    else if(e.getSource() == host) hostName = host.getText();
    else if(e.getSource() == port) portNo = port.getText();
  }

  public void itemStateChanged(ItemEvent e){
    if(e.getItem() == connectionType) connectionLine = connectionType.getSelectedIndex();
  }

  ////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////actionListener--fine

  /////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////keyListener--inizio
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}

  public void keyPressed(KeyEvent e){
    if (e.getKeyCode() == ircam.jmax.utils.Platform.RETURN_KEY){	
      hostName = host.getText();
      portNo = port.getText();
      connectionChoosen();
      setVisible(false);
      dispose();
    }
  }
  ///////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////keyListener--fine
}





