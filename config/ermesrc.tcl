#
#			Ermes startup file


#
# Set the system root directory; the root directory is always got
# thru the system property

set ftsRootDir [systemProperty "root"]

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

if {[info var ftsArch] != "ftsArch"} then { set ftsArch "origin" }
if {[info var ftsMode] != "ftsMode"} then { set ftsMode "opt" }
if {[info var ftsHost] != "ftsHost"} then { set ftsHost "astor" }
if {[info var ftsConnection] != "ftsConnection"} then { set ftsConnection "client" }
if {[info var ftsPort] != "ftsPort"} then { set ftsPort 2000  }
if {[info var ftsName] != "ftsName"} then { set ftsName "fts" }
if {[info var ftsAudioBuffer] != "ftsAudioBuffer"} then { set ftsAudioBuffer 2048 }
if {[info var ftsSampleRate] != "ftsSampleRate"} then { set ftsSampleRate 44100 }
if {[info var ftsMidiPort] != "ftsMidiPort"} then { set ftsMidiPort "FTSmidi" }

# Boot fts

set ftsBinDir $ftsRootDir/fts/bin/$ftsArch/$ftsMode

ftsconnect $ftsBinDir $ftsName $ftsConnection $ftsHost $ftsPort

ucs show welcome
sync

# Load fts1.5 package 

source $ftsRootDir/packages/fts1.5/fts1.5.tcl

# Loading the default config for the machine, audio and midi

source $ftsRootDir/config/audio.tcl
source $ftsRootDir/config/midi.tcl

# Run the start Hooks, by hand !!!
# so we are sure the correct configuration is there ...
#

runHooks start
