
package ircam.jmax.editors.sequence.track;

import java.awt.event.*;
import java.awt.*;

public interface SequenceTextAreaListener
{
    public abstract void widthChanged(int width);   
    public abstract void heightChanged(int height);
    public abstract void endEdit();
}
