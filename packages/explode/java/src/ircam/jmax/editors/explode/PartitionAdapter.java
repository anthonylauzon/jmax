package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

/**
 * the standard, partition-like adapter for a ScrEvent
 * in the Explode database.
 * The x-coordinates corresponds to time, y and label to pitch, lenght to duration.
 * This adapter can also be used to transpose and zoom.
 */
public class PartitionAdapter extends Adapter {
  
  /**
   * constructor.
   * It creates and assigns its mappers (Time, Pitch, Lenght, Label), 
   * and set the initial values for the zoom, transpose and invertion fields.
   */
  public PartitionAdapter(GraphicContext theGc) 
  {
    gc = theGc;
    XMapper = TimeMapper.getMapper();
    YMapper = PitchMapper.getMapper();
    LenghtMapper = DurationMapper.getMapper();
    LabelMapper = PitchMapper.getMapper();
    
    xZoomFactor = 1;
    yZoomFactor = 1;
    xTranspose = 0;
    yTranspose = 0;
    xInvertion = false;
    yInvertion = false;
    itsName = "Standard Adapter";
  }


  /**
   * inherited from Adapter.
   * it returns the X value of the event,
   * making the needed cordinate conversions.
   */
  public int getX(ScrEvent e) 
  {
    int temp = super.getX(e);
    
    if (xInvertion) temp = -temp;

    return (int) ((temp+xTranspose)*xZoomFactor);
  }


  /**
   * inherited from Adapter.
   * Returns the time associated with the value of an X coordinate after
   * the coordinate conversion.
   */
  public int getInvX(int x) 
  {

    if (xInvertion) return (int) (xTranspose -x/xZoomFactor);

    else return (int) (x/xZoomFactor - xTranspose);
    
  }


  /**
   * set the time of the event associated with the graphic X
   */
  public void setX(ScrEvent e, int x) 
  {  
    super.setX(e, getInvX(x));
  }


  /**
   * inherited from Adapter.
   * it returns the Y value of the event,
   * making the needed cordinate conversions.
   */
  public int getY(ScrEvent e) 
  {  
    int temp = super.getY(e);
    
    if (yInvertion) temp = -temp;
    
    return (int) ((temp+yTranspose)*yZoomFactor);  
  }


  /**
   * inherited from Adapter.
   * Returns the pitch associated with the value of an Y coordinate
   */  
  public int getInvY(int y) 
  {
    int temp;

    if (yInvertion) temp = (int) (yTranspose -y/yZoomFactor);
    else temp = (int) (y/yZoomFactor - yTranspose);
  
    return temp;
  }


  /**
   * set the pitch of the event associated with the graphic y
   */
  public void setY(ScrEvent e, int y) 
  {
    super.setY(e, getInvY(y));
  }


  /**
   * inherited from Adapter.
   * returns the lenght value of the event,
   * making the needed cordinate conversions (zooming).
   */
  public int getLenght(ScrEvent e) 
  {
    return (int) (super.getLenght(e)*xZoomFactor);
  }


  /**
   * set the duration of the event associated with the graphic lenght l.
   */
  public  void setLenght(ScrEvent e, int l) 
  {
    super.setLenght(e, (int) (l/xZoomFactor));
  }


  /**
   * From the MappingListener interface.
   * Reassign the mappers according to the change, and ask
   * a repaint of the graphic.
   */
  public void mappingChanged(String graphicName, String scoreName) 
  {

    if (graphicName.equals("y")) 
      {
	if (scoreName.equals("pitch"))
	  YMapper = PitchMapper.getMapper();
	else if (scoreName.equals("duration"))
	  YMapper = DurationMapper.getMapper();
	else if (scoreName.equals("velocity"))
	  YMapper = VelocityMapper.getMapper();
	else if (scoreName.equals("channel"))
	  YMapper = ChannelMapper.getMapper();
      }
    else if (graphicName.equals("lenght")) 
      {
	if (scoreName.equals("pitch"))
	  LenghtMapper = PitchMapper.getMapper();
	else if (scoreName.equals("duration"))
	  LenghtMapper = DurationMapper.getMapper();
	else if (scoreName.equals("velocity"))
	  LenghtMapper = VelocityMapper.getMapper();
	else if (scoreName.equals("channel"))
	  LenghtMapper = ChannelMapper.getMapper();
      }
    else if (graphicName.equals("label")) 
      {
	if (scoreName.equals("pitch"))
	  LabelMapper = PitchMapper.getMapper();
	else if (scoreName.equals("duration"))
	  LabelMapper = DurationMapper.getMapper();
	else if (scoreName.equals("velocity"))
	  LabelMapper = VelocityMapper.getMapper();
	else if (scoreName.equals("channel"))
	  LabelMapper = ChannelMapper.getMapper();
      }

    gc.getGraphicDestination().repaint();
  }
  //------------- Fields

  GraphicContext gc;
}






