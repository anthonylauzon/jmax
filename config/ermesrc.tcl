#
#			Ermes startup file
#

##
## Declare the global variables we manipulate 
## (these files are sources thru a procedure)
##

global jmaxUserDir
global ftsRootDir jmax_sys_pkgPath ftsArch ftsMode ftsHost ftsConnection ftsPort
global ftsName ftsAudioBuffer ftsSampleRate jmax_pkgPath
global ftsMidiPort



#
# Set the system root directory; the root directory is always got
# thru the system property

set ftsRootDir [systemProperty "root"]
##
## Set the system package path 
##

set jmax_sys_pkgPath  "$ftsRootDir/packages"

## 
## Get the boot parameter; command line arguments have the priority, then 
## follow the variables set in ~/.ermesrc, last give some reasonable default
##

if {[systemProperty "ftsArch"] != ""} then { set ftsArch [systemProperty "ftsArch"]}
if {[systemProperty "ftsMode"] != ""} then { set ftsMode [systemProperty "ftsMode"]}
if {[systemProperty "ftsHost"] != ""} then { set ftsMode [systemProperty "ftsHost"]}
if {[systemProperty "ftsConnection"] != ""} then { set ftsConnection [systemProperty "ftsConnection"]}
if {[systemProperty "ftsPort"] != ""} then { set ftsPort [systemProperty "ftsPort"]}
if {[systemProperty "ftsName"] != ""} then { set ftsName [systemProperty "ftsName"]}
if {[systemProperty "ftsAudioBuffer"] != ""} then { set ftsAudioBuffer [systemProperty "ftsAudioBuffer"]}
if {[systemProperty "ftsSampleRate"] != ""} then { set ftsSampleRate [systemProperty "ftsSampleRate"]}
if {[systemProperty "ftsMidiPort"] != ""} then { set ftsMidiPort [systemProperty "ftsMidiPort"]}
if {[systemProperty "jmax_pkgPath"] != ""} then { set jmax_pkgPath [systemProperty "jmax_pkgPath"]}

if [catch {set foo $ftsArch}] then { set ftsArch "origin" }
if [catch {set foo $ftsMode}] then { set ftsMode "opt" }
if [catch {set foo $ftsHost}] then { set ftsHost "astor" }
if [catch {set foo $ftsConnection}] then { set ftsConnection "client" }
if [catch {set foo $ftsPort}] then { set ftsPort 2000}
if [catch {set foo $ftsName}] then {set ftsName "fts" }
if [catch {set foo $ftsAudioBuffer}] then { set ftsAudioBuffer 2048 }
if [catch {set foo $ftsSampleRate}] then { set ftsSampleRate 44100 }
if [catch {set foo $ftsMidiPort}] then { set ftsMidiPort "FTSmidi" }
if [catch {set foo $jmax_pkgPath}] then { set jmax_pkgPath [list $jmaxUserDir $jmaxUserDir/jmax]}


##
## Run the official Ermes splash screen
## to substitute the splash screen, or to disable it,
## just change or comment the following line; also,
## feel free to use the splash command in user packages
##

splash   $ftsRootDir/ermes/images/Splash.gif

#splash   /u/worksta/dececco/./lib/images/kitchen1.gif

## Open the console; for a console less application
## comment the next line
##

openConsole

# Boot fts

set ftsBinDir $ftsRootDir/fts/bin/$ftsArch/$ftsMode

ftsconnect $ftsBinDir $ftsName $ftsConnection $ftsHost $ftsPort

ucs show welcome
sync

# Load fts1.5 package 

package require fts1.5

# Loading the default config for the machine, audio and midi

sourceFile $dir/audio.tcl
sourceFile $dir/midi.tcl

# Run the start Hooks, by hand !!!
# so we are sure the correct configuration is there ...
#

runHooks start



