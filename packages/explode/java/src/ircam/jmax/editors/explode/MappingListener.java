package ircam.jmax.editors.explode;

/**
 * interface of the objects that want to be informed when a
 * graphic mapping changes. Used by the score and its setting panel
 */
public interface MappingListener 
{
  /** callback */
  abstract public void mappingChanged(String graphicName, String scoreName);

}
