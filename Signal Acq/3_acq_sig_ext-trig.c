/* Instantly acquiring a signal on a specific channel, external trigger */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rp.h"


int main(int argc, char **argv){

    /* Print error, if rp_Init() function failed */
    if(rp_Init() != RP_OK){
        fprintf(stderr, "Rp api init failed!\n");
    }

    /* Reset Generation and Acquisition */
    // rp_GenReset();
    rp_AcqReset();

    /* Generation */
    /*LOOB BACK FROM OUTPUT 2 - ONLY FOR TESTING*/
    // rp_GenFreq(RP_CH_1, 20000.0);
    // rp_GenAmp(RP_CH_1, 1.0);
    // rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE);
    // rp_GenOutEnable(RP_CH_1);


    /* Acquisition */
    uint32_t buff_size = 16384;
    float *buff = (float *)malloc(buff_size * sizeof(float));

    // Decimaion factor determines sample rate/Time scale 
    // see this link -> https://redpitaya.readthedocs.io/en/latest/appsFeatures/examples/acquisition/acqRF-samp-and-dec.html
    rp_AcqSetDecimation(RP_DEC_8);
    rp_AcqSetTriggerLevel(RP_CH_1, 0.5); //Trig level in volts
    rp_AcqSetTriggerDelay(0);

    // There is an option to select coupling when using SIGNALlab 250-12
    // rp_AcqSetAC_DC(RP_CH_1, RP_AC);           // enables AC coupling on Channel 1

    // By default LV level gain is selected
    rp_AcqSetGain(RP_CH_1, RP_LOW);              // user can switch gain using this command

    rp_AcqStart();
    sleep(1);

    /* After the acquisition is started some time delay is needed to acquire fresh samples into buffer
    Here we have used a time delay of one second but you can calculate the exact value taking into account buffer
    length and sampling rate*/

    // trigger source is external signal
    // trigger on positive edge when trigger level is reached
    rp_AcqSetTriggerSrc(RP_TRIG_SRC_EXT_PE);
    rp_acq_trig_state_t state = RP_TRIG_STATE_TRIGGERED;

    while(1){
        rp_AcqGetTriggerState(&state);
        if(state == RP_TRIG_STATE_TRIGGERED){
            break;
        }
    }

    // !! OS 2.00 or higher only !! //
    bool fillState = false;
    while(!fillState){
        rp_AcqGetBufferFillState(&fillState);
    }

    rp_AcqGetOldestDataV(RP_CH_1, &buff_size, buff);
    int i;
    for(i = 0; i < buff_size; i++){
        printf("%f\n", buff[i]);
    }

    /* Releasing resources */
    free(buff);
    rp_Release();
    return 0;
}