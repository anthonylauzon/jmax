
package ircam.jmax.toolkit;

/**
 * GraphicContext switch listeners */
public interface SwitchListener {
    abstract public void contextChanged(GraphicContext gc);
    abstract public void contextDestroyed(GraphicContext gc);
}
