typedef struct
{
    int size;
    int n_channels;
    float** buffer; /* n_channels * size */
    int full; /* 0: empty
		 1: full
	      */
    int end_index; /* need for writing */
} dtd_buffer_t;
