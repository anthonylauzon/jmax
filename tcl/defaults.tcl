#
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#  
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  

##
##  jMax system defaults of the system properties
##  

## default audio parameters

set jmaxSampleRate 44100
set jmaxAudioBuffer 2048

## set jMax client/server connection mode

set jmaxConnection "udp"

## set binary execution mode for jMax server executable

set jmaxMode "opt"

## set name for jMax server executable

set jmaxServerName "fts"

## set port number needed for socket connection

set jmaxPort 2000

## default user package path

set jmaxPkgPath "[systemProperty user.home]/jmax"

## default architecture if not specified for host name
## just works if server runns on same machine as client

set jmaxArch "irix65r10k"

## default "UseIcons" value for Swing File Box

set jmaxFastFileBox "false"

## default host 

set jmaxHost "local"

## By default, use the jmax Console

set jmaxNoConsole "false"

##
## Set the system packages path
##

set jmaxSysPkgPath "$jmaxRootDir/packages"

##
## Splash screen setting
##

set jmaxSplashScreen "show"


