package ircam.ftsclient;

public class Test {

  class MyCallback implements FtsCallback {
    public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
    {
      if (argc == 1 && argv[0].isInt())
	System.out.println( argv[0].intValue);
    }

    public void invoke( FtsObject obj, String selector, int argc, FtsAtom[] argv)
    {
      System.out.println( "Callback 2 [" + selector + "] " + obj + " argc=" + argc);
    }
  }

  Test( String hostname)
  {
    server = new FtsServer();

    server.setHostname( hostname);

    try
      {
	server.connect();

	FtsArgs args = new FtsArgs();

	args.clear();
	args.add( "FOO");
	FtsObject o = new FtsObject( server, null, "client_controller", args);

	MyCallback mc = new MyCallback();

	FtsObject.registerCallback( o.getClass(), "int", mc);
	FtsObject.registerCallback( o.getClass(), mc);

	int count = 42;

	while( true)
	  {
	    o.send( count++);

	    Thread.sleep( 1000);
	  }
      }
    catch( Exception e)
      {
	e.printStackTrace();
      }
  }

  public static void main( String args[])
  {
    new Test( args[0]);
  }

  private FtsServer server;
}
