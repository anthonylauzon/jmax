##
## The config command
##
##
## config define <name> <doc> <body>
## config set    <name>
## config names
##
##



proc config {cmd {arg1 "void"} {arg2 "void"} {arg3 "void"}} {
    switch $cmd {
	define    {configDefine $arg1 $arg2 $arg3}
	set       {configSet    $arg1}
	list      {configNames      }
    }
}


proc configDefine {name doc body} {
    global jmaxConfigCommand jmaxConfigComment

    set jmaxConfigCommand($name) $body
    set jmaxConfigComment($name) $doc
}


proc configSet {name} {
    global jmaxConfigCommand jmaxConfigComment

    puts $jmaxConfigComment($name)
    uplevel #0 eval $jmaxConfigCommand($name)
}


proc configNames {} {
    global jmaxConfigCommand jmaxConfigComment

    if [array exists jmaxConfigCommand] {
	set list [array names jmaxConfigCommand]

	foreach name $list {
	    puts "$name $jmaxConfigComment($name)"
	}
    }
}
