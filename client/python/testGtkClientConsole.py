#!/usr/bin/env python2.2
import sys
import time
import gtk
import gobject

from new import classobj

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
    

class FtsConnection(FtsObject):
    # private static member
    __fts_connection_invalid = -1
    __fts_connection_anything = 0
    __fts_connection_message = 1
    __fts_connection_value = 2
    __fts_connection_audio = 3
    __fts_connection_audio_active = 4
    
    # public static member

    
class FtsConsoleStream(FtsObject):
    def __init__(self, window, box, serverConnection, parent, *args):
        # Call parent class constructor
        FtsObject.__init__(self, serverConnection, parent, *args)
        # Create ConsoleArea:
        self.console_area = ConsoleArea(window, box)

    def print_line(self, args):
        self.console_area.post(args)
        return


class FtsPatcherObject(FtsObject):
    def __init__(self, serverConnection, parent, *args):
        self.object_list = []
        
    def addObject(self, args):
        print 'FtsPatcherObject: addObject Received'
        print 'Object ID: ', args.getInt(0)
        print 'x position: ', args.getInt(1)
        print 'y position: ', args.getInt(2)
        print 'width : ', args.getInt(3)
        print 'heights : ', args.getInt(4)
        print 'numIns : ', args.getInt(5)
        print 'numOuts : ', args.getInt(6)
        print 'layer : ', args.getInt(7)
        print 'Error Description: ', args.getString(8)
        print 'Classname: ', args.getString(9)
        print 'IsTemplate: ', args.getInt(10)
        # create object and add it to object list
        
    def addConnection(self, args):
        print 'FtsPatcherObject: addConnection'



class JMaxApplication:
    rootPatcher = None
    connection = None
    
    def destroy(self, widget, data=None):
        return gtk.mainquit()    

    def file_ok_sel(self, widget, fileSel):
        fileName = fileSel.get_filename()
        args = []
        args.append(FtsRawString(fileName))
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
        self.console_stream.send("set_default")
        self.clientPatcher = FtsObject(connection, rootPatcher, 1)
        JMaxApplication.connection = connection
        JMaxApplication.rootPatcher = rootPatcher
    
    
    def start(self):
        gtk.threads_enter()
        gtk.mainloop()
        gtk.threads_leave()

    def getRootPatcher():
        return JMaxApplication.rootPatcher

    getRootPatcher = staticmethod(getRootPatcher)
    
    def getFtsServer():
        return JMaxApplication.connection

    getFtsServer = staticmethod(getFtsServer)




        

myApp = JMaxApplication()
myApp.start()

sys.exit(0)    
