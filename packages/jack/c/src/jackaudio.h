#ifndef JACKAUDIO_H_
#define JACKAUDIO_H_ 1





typedef struct
{
    fts_audioport_t head;
    /* TODO: 
       Change structure if we want several input and output .
       But maybe it should be several jackaudioport, in this case we have to choose
       unique jack_client name */
    jack_port_t* input_port;
    jack_port_t* output_port;
    /* TODO:
       If we add several input/output  port, we should want several input/output buffer
    */
    float* input_buffer;
    float* output_buffer;
    /* TODO:
       Frame size could be changed by jack, we need to add a callback for buffer size 
       change */
    int nframes;
    /* TODO:
       sampling rate could be changed by jack, we need to add a callback for sampling 
       rate change */
    unsigned long samplingRate;

} jackaudioport_t;


fts_class_t* jackaudioport_type;

jack_client_t* jackaudiomanager_get_jack_client(void);

#endif /* JACKAUDIO_H_ */
