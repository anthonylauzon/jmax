package ircam.jmax.editors.patcher;

import java.awt.*;
import java.lang.*;
import java.util.*;
import javax.swing.*;
import ircam.jmax.*;
public class ScaleTransform {

    private float scaleX;
    private float invScaleX;
    private float scaleY;
    private float invScaleY;
    
    private static ScaleTransform instance;
    public static ScaleTransform getInstance()
    {
	if(instance == null) instance = new ScaleTransform();
	return instance;
    }

    public ScaleTransform()
    {
	recomputeScaleFactors(defaultFontName, defaultBaseSize);
    }

    public static void createScaleTransform(float xfactor, float yfactor)
    {
	instance = new ScaleTransform(xfactor, yfactor);
    }
    public static void createScaleTransform()
    {
	instance = new ScaleTransform();
    }

    public ScaleTransform(float xfactor, float yfactor)
    {
	fontName = defaultFontName;
	baseSize = defaultBaseSize;
	scaleX = xfactor;   

	initFontMetrics();

	invScaleX = 1/xfactor;	
	scaleY = yfactor;
	invScaleY = 1/yfactor;
    }

    public void recomputeScaleFactors(String fName, int bSize)
    {
	fontName = fName;
	baseSize = bSize;
	
	initFontMetrics();
	
	scaleX = computeScaleX();   
	invScaleX = 1/scaleX;	
	scaleY = computeScaleY();
	invScaleY = 1/scaleY;
    }

    public void fontBaseChanged(int bSize)
    {
	recomputeScaleFactors(fontName, bSize);
	ListModel windows = MaxWindowManager.getWindowManager().getWindowList();
	for(int i = 0; i<windows.getSize();i++)
	    ((ErmesSketchWindow)windows.getElementAt(i)).fontBaseChanged(bSize);
    }

    public final int scaleX( float x)
    {
	return Math.round(x*scaleX);
    }

    public final float invScaleX( int x)
    {
	return x*invScaleX;
    }

    public final int scaleY( float y)
    {
	return Math.round(y*scaleY);
    }

    public final float invScaleY( int y)
    {
	return y*invScaleY;
    }

    private float computeScaleX()
    {
	String str;
	StringDescription d;
	float temp; 
	float maxs[] = {(float)0.0, (float)0.0,(float)0.0, (float)0.0, (float)0.0, (float)0.0};
	//find max for every font size
	for(int i = 0 ; i< xReferences.length; i++ )
	    {
		d = xReferences[i];
		for(int j = 0; j < numSizes; j++)
		{
		  temp = (float)standardFontMetrics[j].stringWidth(d.string)/(float)d.width(j);
		  if(temp>maxs[j]) maxs[j] = temp;
		}
	    }
	//medium of maxs[i] without maximum value
	float medium = (float)0.0;
	float max = (float)0.0;
	int div = 0;
	for(int i =0; i<numSizes; i++)
	    if(maxs[i]>max) max = maxs[i];
	for(int i =0; i<numSizes; i++)
	    if(maxs[i]<max) 
		{
		    medium+=maxs[i];
		    div++;
		}
	medium = medium/div;
	return medium;
   }

    private float computeScaleY()
    {
	float temp, medium = (float)0.0;
	float max = (float)0.0;	
	for(int j = 0; j < numSizes; j++)
	{
	    temp = (float)(standardFontMetrics[j].getHeight()/(float)yReferences[j]);
	    if(temp>max) max = temp;
	    medium +=temp;
	}
	medium = (medium-max)/(numSizes-1);
	//return max;
	return medium;
    }

    void initFontMetrics()
    {
	int start = getBaseSizeIndex();
	for(int i=0; i<numSizes; i++)
	    standardFontMetrics[i] = Toolkit.getDefaultToolkit().
		getFontMetrics(new Font(fontName, Font.PLAIN, standardSizes[start+i]));
    }

    int getBaseSizeIndex()
    {
	for(int i=0; i</*standardSizes.length*/11; i++)
	    if(standardSizes[i] == baseSize) return i;
	return -1;
    }

    public StringDescription xReferences[] = {
	new StringDescription("a", 4, 5, 7, 8, 9, 13),
	new StringDescription("b", 5, 6, 7, 8, 11, 14),
	new StringDescription("c", 4, 5, 7, 8, 10, 12),
	new StringDescription("d", 5, 6, 7, 8, 11, 14),
	new StringDescription("e", 4, 5, 7, 8, 10, 13),
	//new StringDescription("f", 3, 4, 3, 3, 6, 8),
	new StringDescription("g", 5, 6, 7, 8, 11, 14),
	new StringDescription("h", 5, 6, 7, 8, 10, 13),
	new StringDescription("i", 2, 2, 3, 3, 4, 6),
	new StringDescription("j", 2, 2, 3, 3, 4, 6),
	new StringDescription("k", 4, 5, 6, 7, 9, 12),
	new StringDescription("l", 2, 2, 3, 3, 4, 6),
	new StringDescription("m", 6, 8, 9, 11, 14, 20),
	new StringDescription("n", 5, 6, 7, 8, 10, 14),
	new StringDescription("o", 5, 6, 7, 8, 11, 13),
	new StringDescription("p", 5, 6, 7, 8, 11, 14),
	new StringDescription("q", 5, 6, 7, 8, 11, 14),
	new StringDescription("r", 3, 4, 4, 5, 6, 9),
	new StringDescription("s", 4, 5, 6, 8, 9, 12),
	//new StringDescription("t", 3, 4, 3, 4, 6, 8),
	new StringDescription("u", 4, 5, 7, 8, 10, 14),
	new StringDescription("v", 5, 6, 7, 8, 10, 13),
	new StringDescription("x", 5, 6, 6, 7, 10, 12),
	new StringDescription("y", 4, 5, 7, 7, 10, 13),
	new StringDescription("w", 6, 8, 9, 10, 14, 18),
	new StringDescription("z", 4, 5, 6, 7, 9, 12),
	new StringDescription("A", 6, 7, 9, 11, 12, 17),
	new StringDescription("B", 6, 7, 8, 9, 13, 17),
	new StringDescription("C", 6, 8, 9, 10, 14, 18),
	new StringDescription("D", 6, 8, 9, 10, 13, 18),
	new StringDescription("E", 6, 7, 8, 9, 11, 16),
	new StringDescription("F", 5, 6, 8, 9, 11, 14),
	new StringDescription("G", 6, 8, 9, 11, 14, 19),
	new StringDescription("H", 6, 8, 9, 10, 13, 18),
	new StringDescription("I", 2, 3, 3, 5, 6, 8),
	new StringDescription("J", 4, 5, 7, 8, 10, 13),
	new StringDescription("K", 6, 7, 8, 10, 13, 18),
	new StringDescription("L", 5, 6, 7, 8, 10, 14),
	new StringDescription("M", 7, 9, 11, 13, 16, 21),
	new StringDescription("N", 6, 8, 9, 10, 13, 18),
	new StringDescription("O", 6, 8, 10, 11, 15, 18),
    	new StringDescription("P", 6, 7, 8, 9, 12, 16),
	new StringDescription("Q", 6, 8, 10, 11, 15, 18),
	new StringDescription("R", 6, 7, 8, 9, 12, 17),
	new StringDescription("S", 6, 7, 8, 9, 13, 16),
	new StringDescription("T", 4, 5, 7, 9, 12, 16),
	new StringDescription("U", 6, 8, 8, 10, 13, 18),
	new StringDescription("V", 6, 7, 9, 11, 14, 17),
	new StringDescription("X", 6, 7, 9, 10, 13, 17),
	new StringDescription("Y", 6, 7, 9, 9, 14, 16),
	new StringDescription("W", 7, 9, 11, 15, 18, 22),
	new StringDescription("Z", 6, 7, 9, 9, 12, 15),
	new StringDescription("0", 5, 6, 7, 8, 10, 13),
	new StringDescription("1", 5, 6, 7, 8, 10, 13),
	new StringDescription("2", 5, 6, 7, 8, 10, 13),
	new StringDescription("3", 5, 6, 7, 8, 10, 13),
	new StringDescription("4", 5, 6, 7, 8, 10, 13),
	new StringDescription("5", 5, 6, 7, 8, 10, 13),
	new StringDescription("6", 5, 6, 7, 8, 10, 13),
	new StringDescription("7", 5, 6, 7, 8, 10, 13),
	new StringDescription("8", 5, 6, 7, 8, 10, 13),
	new StringDescription("9", 5, 6, 7, 8, 10, 13),
	new StringDescription("+", 5, 6, 7, 9, 10, 14),
	new StringDescription("-", 5, 6, 7, 8, 10, 13),
	new StringDescription("*", 3, 4, 5, 7, 7, 10),
	new StringDescription("$", 5, 6, 7, 8, 10, 13),
	new StringDescription("#", 5, 6, 7, 8, 10, 14), 
	new StringDescription("~", 6, 7, 7, 8, 10, 14), 
	new StringDescription("<", 5, 6, 7, 8, 10, 15),
	new StringDescription(">", 5, 6, 7, 8, 10, 15),
	new StringDescription("%", 7, 9, 11, 12, 16, 22),
	new StringDescription("(", 3, 4, 4, 5, 6, 8),
	new StringDescription(")", 3, 4, 4, 5, 6, 8), 
	new StringDescription(".", 2, 3, 3, 3, 5, 6),
	new StringDescription(",", 2, 3, 4, 3, 5, 6),
	new StringDescription(":", 2, 3, 3, 3, 5, 6),
	new StringDescription(";", 2, 3, 3, 4, 5, 6),
	new StringDescription(" ", 2, 3, 4, 4, 5, 6)         
	    };

    public int yReferences[] = { 11, 13, 15, 17, 22, 28};

    public static final String defaultFontName = "SansSerif";    
    public static final int defaultBaseSize = 8;    
    
    public String fontName;
    public int baseSize;
    
    public final int numSizes = 6;
    public int standardSizes[] = {8, 10, 12, 14, 18, 24, 36, 48, 60, 72, 84};  

    public FontMetrics standardFontMetrics[] = new FontMetrics[numSizes];

    public class StringDescription
    {
	public String string;
	public int[] widths = new int[6];
    
	public StringDescription(String str, int s8, int s10, int s12, int s14, int s18, int s24)
	{
	    string = str; 	
	    widths[0] = s8; 
	    widths[1] = s10; 
	    widths[2] = s12; 
	    widths[3] = s14; 
	    widths[4] = s18; 
	    widths[5] = s24;
	}
	public int width(int index)
	{
	    if(index < 6)
		return widths[index];
	    else
		return 0;
	}
    }
}
