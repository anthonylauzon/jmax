package ircam.jmax.editors.frobber;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

public interface KeyEventClient extends KeyListener {
  public void keyInputGained();
  public void keyInputLost();
}

  
