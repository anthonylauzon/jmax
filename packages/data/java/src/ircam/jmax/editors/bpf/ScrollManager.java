
package ircam.jmax.editors.bpf;

import javax.swing.*;
import java.awt.*;

public interface ScrollManager {

    public void scrollIfNeeded(int time);
    public int scrollBy(int x, int y);
    public void scrollToValue(int value);
    public void makeVisible(BpfPoint point);
    public Rectangle getViewRectangle();
}
