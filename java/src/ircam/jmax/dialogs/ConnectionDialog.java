
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
 */
public class ConnectionDialog extends Dialog implements KeyListener, ActionListener, ItemListener{
  Frame parent;
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
    

  public ConnectionDialog(Frame dw) {
    super(dw, "FTS connection", false);

    parent = dw;

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
  }

  /*public boolean action(Event event, Object arg) {
    if ( event.target == okButton) {
    hostName = host.getText();
    portNo = port.getText();
    MaxApplication.getApplication().ObeyCommand(MaxApplication.CONNECTION_CHOOSEN);
    setVisible(false);
    }
    else if ( event.target == cancelButton) {
    setVisible(false);
    }
    else if ( event.target == connectionType) {
    connectionLine = connectionType.getSelectedIndex();
    }
    else if ( event.target == host) {
    hostName = host.getText();
    //	Connection choose action
    }
    else if ( event.target == port) {
    portNo = port.getText();
    //	Connection choose action
    }
    return true;
    }*/

  ////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////actionListener--inizio

  public void actionPerformed(ActionEvent e){    
    if (e.getSource() == okButton){
      hostName = host.getText();
      portNo = port.getText();
      MaxApplication.getApplication().ObeyCommand(MaxApplication.CONNECTION_CHOOSEN);
      setVisible(false);
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

  /* public boolean keyDown(Event evt,int key) {
     if (key == ircam.jmax.utils.Platform.RETURN_KEY){	
     hostName = host.getText();
     portNo = port.getText();
     MaxApplication.getApplication().ObeyCommand(MaxApplication.CONNECTION_CHOOSEN);
     setVisible(false);
     return true;
     }
     return false;
     }*/


  /////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////keyListener--inizio
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}

  public void keyPressed(KeyEvent e){
    if (e.getKeyCode() == ircam.jmax.utils.Platform.RETURN_KEY){	
      hostName = host.getText();
      portNo = port.getText();
      MaxApplication.getApplication().ObeyCommand(MaxApplication.CONNECTION_CHOOSEN);
      setVisible(false);
    }
  }
  ///////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////keyListener--fine
}
