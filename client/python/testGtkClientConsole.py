#!/usr/bin/env python2.2
import sys
import time
import gtk
import gobject

from ircam.fts.client import *

class ConsoleArea:
    def hello(self, widget, data=None):
        print "Hello"

    def post(self, message):
        print "Post with message", message
        textBuffer = self.text.get_buffer()
        textBuffer.insert(textBuffer.get_end_iter(), message)
        textBuffer.insert(textBuffer.get_end_iter(), "\n")
    
    def __init__(self, window, box):
        print "Console Area init"
        # Create a box to contains text and button
        self.window = window
        self.box = box

        self.button = gtk.Button("Start")
        self.button.connect("clicked", self.hello)
        
        self.box.pack_start(self.button, gtk.TRUE, gtk.TRUE, 0)
        
        self.text = gtk.TextView()
        self.box.pack_start(self.text, gtk.TRUE, gtk.TRUE, 0)
        self.button.show()
        self.text.show()
        self.box.show()
        self.window.show()
    

class FtsConsoleStream(FtsObject):
    def __init__(self, window, box, serverConnection, parent, *args):
        # Call parent class constructor
        FtsObject.__init__(self, serverConnection, parent, *args)
        # Create ConsoleArea:
        self.console_area = ConsoleArea(window, box)


class FtsConsoleStreamHandler:
    def invoke(self, obj, args):
        obj.console_area.post(args.getString(0))

class JMaxApplication:
    def destroy(self, widget, data=None):
        return gtk.mainquit()    

    def file_ok_sel(self, widget, fileSel):
        fileName = fileSel.get_filename()
        args = FtsArgs()
        args.clear()
        args.addString(fileName)
        self.clientPatcher.send("load", args)
        
    def menuitem_response(self, widget, string):
        fileSel = gtk.FileSelection()        
        fileSel.ok_button.connect("clicked", self.file_ok_sel, fileSel)
        fileSel.show()
        
    def __init__(self):
        gtk.threads_init()
        # Create window
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect("destroy", self.destroy)
        self.window.set_border_width(10)

        # Create menus
        menu  = gtk.Menu()
        buf = "Open File"
        menu_items=  gtk.MenuItem("Start Patch")
        menu.append(menu_items)
        menu_items.connect("activate", self.menuitem_response, buf)
        menu_items.show()

        root_menu = gtk.MenuItem("File")
        root_menu.show()

        root_menu.set_submenu(menu)
        
        vbox = gtk.VBox(gtk.FALSE,0)
        self.window.add(vbox)
        vbox.show()

        menu_bar = gtk.MenuBar()
        vbox.pack_start(menu_bar, gtk.FALSE, gtk.FALSE, 0)
        menu_bar.append(root_menu)
        menu_bar.show()


        connection = FtsSocketConnection()
        rootPatcher = FtsObject(connection, None, 0)
        connection.putObject(0, rootPatcher)
        
        self.console_stream = FtsConsoleStream(self.window, vbox, connection, rootPatcher, "console_stream")
        FtsObject.registerMessageHandler(self.console_stream.__class__,"print_line", FtsConsoleStreamHandler())
        self.console_stream.send("set_default")
        self.rootPatcher = rootPatcher
        self.connection = connection
        self.clientPatcher = FtsObject(self.connection, self.rootPatcher, 1)
        
    def start(self):
        gtk.threads_enter()
        gtk.mainloop()
        gtk.threads_leave()



        

#clientPatcher = FtsObject(connection, rootPatcher, 1);
#args = FtsArgs()
#args.clear()
# add path to patch to load
#args.addString(sys.argv[1])
# send load message to fts
#clientPatcher.send("load", args)


myApp = JMaxApplication()
myApp.start()

sys.exit(0)    
