import java.io.*;

public class SilkMethodPrinter implements MethodPrinter {

    public void printConstructor(String f, String c, String[] a) {
	System.out.print("(define " + f + " (constructor \"" + c + "\"");
	for (int i = 0; i < a.length; i++) {
	    System.out.print(" \""+ a[i] + "\"");
	}
	System.out.println("))");
    }

    public void printMethod(String f, String c, String m, String t, String r, String[] a) {
	System.out.print("(define " + f + " (method \"" + m + "\" \"" + c + "\"");
	for (int i = 0; i < a.length; i++) {
	    System.out.print(" \""+ a[i] + "\"");
	}
	System.out.println("))");
    }
}

