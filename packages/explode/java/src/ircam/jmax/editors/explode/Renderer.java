package ircam.jmax.editors.explode;

import java.awt.*;
/**
 * A graphic representation of a list of events.
 * It has the responsibility of drawing the background and
 * events.
 */
public interface Renderer {
  abstract public void render(Graphics g);
  abstract public EventRenderer getEventRenderer();
}
