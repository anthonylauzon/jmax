#
#  jMax startup file
#

# open the console

openConsole

# run the official jMax splash screen 
# if not supressed by the user in .jmaxrc

if {$jmaxSplashScreen != "hide"} then {
  splash $jmaxRootDir/images/Splash.gif [getMaxVersion]
}

# start jMax server

set jmaxBinDir "$jmaxRootDir/fts/bin/$jmaxArch/$jmaxMode"

if {$jmaxConnection == "tcl"} {
  puts "jMax starting server on $jmaxHost via TCP/IP"
} elseif {$jmaxConnection == "udp"} {
  puts "jMax starting server on $jmaxHost via UDP"
} elseif {$jmaxConnection == "socket"} {
  puts "jMax connecting to server on $jmaxHost via TCP/IP port $jmaxPort"
}

puts "jMax Host type $jmaxHostType"

if {$jmaxMode == "debug"} {
  puts "jMax in DEBUG mode"
}

ftsconnect $jmaxBinDir $jmaxServerName $jmaxConnection $jmaxHost $jmaxPort

# hello server
ucs show welcome
sync

# load gui server side objects
package require guiobj
package require system

#
# load installation default packages
# Use sourceFile as a protection against user errors
#
sourceFile $jmaxRootDir/config/packages.tcl

jmaxSetSampleRate $jmaxSampleRate
jmaxSetAudioBuffer $jmaxAudioBuffer

# run the start Hooks, by hand !!!
# so we are sure the correct configuration is there ...

runHooks start

# if profiling is on, close audio device 
# and install the profile device; this after
#the when start, i.e. including user configuration

if {[systemProperty "profile"] == "true"} {
    puts "running with pseudo audio device for profiling"
    resetAudioOut
    resetAudioIn
    openDefaultAudioOut profile
} 


