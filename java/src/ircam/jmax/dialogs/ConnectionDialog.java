
package ircam.jmax.dialogs;

import ircam.jmax.*;
import java.awt.*;
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
public class ConnectionDialog extends Dialog {
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
    p11.add( connectionType);
    p1.add("North", p11);
			
    //	host and port CONTROLS //
    Panel p12 = new Panel();
    p12.setLayout(new BorderLayout());
				
    Panel p121 = new Panel();
    p121.setLayout(new FlowLayout(FlowLayout.LEFT));
					
    p121.add(new Label("Host name"));
    host = new TextField("", 20);
    p121.add(host);
    p12.add("North",p121);
				
    Panel p122 = new Panel();
    p122.setLayout(new FlowLayout(FlowLayout.LEFT));
    p122.add(new Label("Port num."));
    port = new TextField("2000");
    p122.add(port);
		        
    p12.add("West", p122);        
    p1.add("West",p12);	////        

    add("West", p1);	////        

    //Create south section.
    Panel p2 = new Panel();
    p2.setLayout(new BorderLayout());
	        
    Toolkit toolkit = Toolkit.getDefaultToolkit();
    Image aImage1 = toolkit.getImage("images/OKButtonUp.gif");
    Image aImage2 = toolkit.getImage("images/OKButtonDown.gif");
    //okButton = new Button(aImage1, aImage2, this); nice try, Riccardo
	        
    okButton = new Button("OK");
    okButton.setBackground(Color.white);
    p2.add("East", okButton);
    cancelButton = new Button("Cancel");
    cancelButton.setBackground(Color.white);
    p2.add("West", cancelButton);
    add("South", p2);

    //Initialize this dialog to its preferred size.
    pack();

  }

  public boolean action(Event event, Object arg) {

    if ( event.target == okButton) {
      //	Ok action
      hostName = host.getText();
      portNo = port.getText();
      MaxApplication.getApplication().ObeyCommand(MaxApplication.CONNECTION_CHOOSEN);
      hide();
    }
    else if ( event.target == cancelButton) {
      //	Cancel action
      hide();
    }
    else if ( event.target == connectionType) {
      //	Connection choose action
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
  }
    
  public boolean keyDown(Event evt,int key) {
    if (key == ircam.jmax.utils.Platform.RETURN_KEY){	
      hostName = host.getText();
      portNo = port.getText();
      MaxApplication.getApplication().ObeyCommand(MaxApplication.CONNECTION_CHOOSEN);
      hide();
      return true;
    }
    return false;
  }
}
