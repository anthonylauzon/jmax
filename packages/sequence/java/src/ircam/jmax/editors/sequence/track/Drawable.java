
package ircam.jmax.editors.sequence.track;

import ircam.jmax.toolkit.*;

/**
 * Interface of objects that can be drawn by a delegate object - the renderer */
public interface Drawable 
{
    /**
     * Returns the renderer to be used for this object */
    public abstract ObjectRenderer getRenderer();
}
