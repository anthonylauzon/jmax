
package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.track.*;
import javax.swing.*;
import java.awt.*;

public interface ScrollManager {

    public void scrollIfNeeded(int time);
    public int scrollBy(int x, int y);
    public void scrollToValue(int value);
    public void makeVisible(TrackEvent evt);
    public Rectangle getViewRectangle();
}
