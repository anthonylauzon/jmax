####################################################################
#
#  jMax startup file
#

# declare the global variables we manipulate 
# (these files are sources thru a procedure)
global jmaxRootDir 
global jmaxSysPkgPath 
global jmaxHostArch
global jmaxHost jmaxConnection jmaxPort
global jmaxArch jmaxMode jmaxServerName
global jmaxMidiPort jmaxPkgPath
global jmaxSplashScreen
global jmaxFastFileBox

# set the system root directory; 
# the root directory is always got thru the system property
set jmaxRootDir [systemProperty "root"]

# set the system package path 
set jmaxSysPkgPath "$jmaxRootDir/packages"


##########################################################################
#
#  get the boot properties from command line arguments
#  (values overwrite by settings in ~/.jmaxrc)
#

# set server connection to commandline argument
if {[systemProperty "jmaxHost"] != ""} then { set jmaxHost [systemProperty "jmaxHost"]}
if {[systemProperty "jmaxConnection"] != ""} then { set jmaxConnection [systemProperty "jmaxConnection"]}
if {[systemProperty "jmaxPort"] != ""} then { set jmaxPort [systemProperty "jmaxPort"]}

# set server executable to commandline argument
if {[systemProperty "jmaxArch"] != ""} then { set jmaxArch [systemProperty "jmaxArch"]}
if {[systemProperty "jmaxMode"] != ""} then { set jmaxMode [systemProperty "jmaxMode"]}
if {[systemProperty "jmaxServerName"] != ""} then { set jmaxServerName [systemProperty "jmaxServerName"]}

# set MIDI configuration to commandline argument
if {[systemProperty "jmaxMidiPort"] != ""} then { set jmaxMidiPort [systemProperty "jmaxMidiPort"]}

# misc settings
if {[systemProperty "jmaxPkgPath"] != ""} then { set jmaxPkgPath [systemProperty "jmaxPkgPath"]}
if {[systemProperty "jmaxSplashScreen"] != ""} then { set jmaxSplashScreen [systemProperty jmaxSplashScreen]}

# set MIDI configuration to commandline argument
if {[systemProperty "jmaxMidiPort"] != ""} then { set jmaxMidiPort [systemProperty "jmaxMidiPort"]}

# set Swing File Box "Fast"
if {[systemProperty "jmaxFastFileBox"] != ""} then { set jmaxFastFileBox [systemProperty "jmaxFastFileBox"]}

# load installation defaults
source $jmaxRootDir/config/defaults.tcl


##########################################################################
#
#  set installation defaults for all undefined properties
#  (when not given in commandline nor  ~/.jmaxrc)
#

# set server connection to defaults
if [catch {set foo $jmaxHost}] then { set jmaxHost $jmaxDefaultHost}
if [catch {set foo $jmaxConnection}] then { set jmaxConnection $jmaxDefaultConnection}
if [catch {set foo $jmaxPort}] then { set jmaxPort $jmaxDefaultPort}

# set server server executable to defaults
# "jmaxArch" is set to default for host name or to absolute default
if [catch {set foo $jmaxArch}] then { 
  if [catch {set jmaxArch $jmaxHostArch($jmaxHost)}] then { set jmaxArch $jmaxDefaultArch}
}
if [catch {set foo $jmaxMode}] then { set jmaxMode $jmaxDefaultMode}
if [catch {set foo $jmaxServerName}] then {set jmaxServerName $jmaxDefaultServerName}

# set MIDI configuration defaults
if [catch {set foo $jmaxMidiPort}] then { set jmaxMidiPort $jmaxDefaultMidiPort}

# misc defaults
if [catch {set foo $jmaxPkgPath}] then { set jmaxPkgPath $jmaxDefaultPkgPath}

# File Box
if [catch {set foo $jmaxFastFileBox}] then { set jmaxFastFileBox $jmaxDefaultFastFileBox}
# the following is done so it can be accessed from Java
setSystemProperty "jmaxFastFileBox" $jmaxFastFileBox

# hard coded defaults
if [catch {set foo $jmaxSplashScreen}] then { set jmaxSplashScreen "show"}

# load UCS command wrappers
sourceFile $jmaxRootDir/tcl/ucs.tcl

##########################################################################
#
#  startup actions
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

if {$jmaxConnection == "client"} {
  puts "jMax starting server on \"$jmaxHost\" via TCP/IP"
} elseif {$jmaxConnection == "udp"} {
  puts "jMax starting server on \"$jmaxHost\" via UDP"
} elseif {$jmaxConnection == "socket"} {
  puts "jMax connecting to server on \"$jmaxHost\" via TCP/IP port $jmaxPort"
}

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

# load installation default packages
sourceFile $jmaxRootDir/config/packages.tcl

ucs set param sampling_rate $jmaxDefaultSampleRate
ucs set param fifo_size $jmaxDefaultAudioBuffer

# load installation default audio configuration
# if profiling is off
if {[systemProperty "profile"] == "true"} {
  puts "running with pseudo audio device for profiling"
  ucs open device out~ prof_out as prof_dac channels 2 
  ucs default out~ prof_out
} else {
  sourceFile $jmaxRootDir/config/audio.tcl
}

# loading installation default MIDI configuration
sourceFile $jmaxRootDir/config/midi.tcl

# run the start Hooks, by hand !!!
# so we are sure the correct configuration is there ...
runHooks start



