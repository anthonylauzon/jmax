package ircam.jmax.toolkit;

/**
 * This interface express the ability, for the implementing classes,
 * to switch the graphic context of the toolkit interaction.
 * Unlike the fixed paradigm (One editor instance = one graphic context = one window),
 * these objects can trigger a context-switch in situation not necessarily
 * related to windowActivate events.
 * These objects are somehow "active objects", they are usually listening
 * the triggering source (ex., the ComponentContextSwitcher class listens to mouseEntered events
 * in a given Component) and communicate the change to listeners. 
 * The user must prevent triggering conflicts (i.e. the context should be 
 * in every moment uniquely determined).
 */

public interface ContextSwitcher {

    /*
     * Ask to be informed by this switcher when Graphic context changes */
    abstract public void addSwitchListener(SwitchListener listener);
    
    /**
     * Remove the given listener */
    abstract public void removeSwitchListener(SwitchListener listener);

    /**
     * Get the context associated to this switcher*/
    public abstract GraphicContext getGraphicContext();


    /**
     * Sets the graphic context associated to this switcher. */
    public abstract void setGraphicContext(GraphicContext gc);
}

