
package ircam.jmax.editors.sequence;

import javax.swing.*;

public interface ScrollManager {

    public void scrollIfNeeded(int time);
    public int scrollBy(int x, int y);
}
