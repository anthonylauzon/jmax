//
//  JMaxWrapper.java
//  jMaxProject
//
//  Created by riccardo borghesi on Fri Sep 06 2002.
//  Copyright (c) 2002 __Ircam__. All rights reserved.
//

import java.awt.*;
import com.apple.mrj.*;
import ircam.jmax.JMaxApplication;

public class JMaxWrapper {

 public static void main( String args[]) 
  {
    initMacOSXApplication();
    JMaxApplication.main( args);
  }
  
 public static void initMacOSXApplication()
  {
    MRJApplicationUtils.registerQuitHandler( new MRJQuitHandler(){
        public void handleQuit() {
            JMaxApplication.Quit();
            throw new IllegalStateException("Quit Pending User Confirmation");
        }	
    });
  }
}
