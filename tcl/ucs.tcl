##########################################################################
#
#  UCS command wrappers
#  

proc dataDirectory {dir} {
  global DataDirectory

  lappend DataDirectory $dir
  set path [join $DataDirectory ":"]
  ucs set defaultpath $path
}

proc jmaxSetSampleRate {rate} {
  global JmaxSampleRate

  set JmaxSampleRate $rate
  ucs set param sampling_rate $rate
}

proc jmaxGetSampleRate {} {
  global JmaxSampleRate

  return $JmaxSampleRate
}

proc jmaxSetAudioBuffer {size} {
  global JmaxAudioBuffer

  set JmaxAudioBuffer $size
  ucs set param fifo_size $size
}

proc jmaxGetAudioBuffer {} {
  global JmaxAudioBuffer

  return $JmaxAudioBuffer
}

