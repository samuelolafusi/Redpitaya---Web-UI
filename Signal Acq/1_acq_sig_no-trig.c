/* Instantly acquiring a signal on a specific channel, no trigger */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rp.h"

#define MAX_SAMPLES 16384  // Hardware limit per acquisition
#define TOTAL_SAMPLES 16384  // Total samples desired

int main(int argc, char **argv) {

    /* Initialize Red Pitaya */
    if (rp_Init() != RP_OK) {
        fprintf(stderr, "Rp api init failed!\n");
        return 1;
    }

    /* Reset Generation and Acquisition */
    // rp_GenReset();
    rp_AcqReset();

    /* Generation */
    /*LOOB BACK FROM OUTPUT 2 - ONLY FOR TESTING*/
    //rp_GenFreq(RP_CH_1, 20000.0);
    //rp_GenAmp(RP_CH_1, 1.0);
    //rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE);
    //rp_GenOutEnable(RP_CH_1);

    /* Acquisition Setup */
    uint32_t buff_size = MAX_SAMPLES;  // Single acquisition buffer size
    uint32_t total_samples = TOTAL_SAMPLES;  // Total samples to collect
    uint32_t num_acquisitions = (total_samples + buff_size - 1) / buff_size;  // Ceiling division
    float *buff = (float *)malloc(buff_size * sizeof(float));

    if (buff == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        rp_Release();
        return 1;
    }

    FILE *file = fopen("samples.txt", "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file!\n");
        free(buff);
        rp_Release();
        return 1;
    }

    // Decimaion factor determines sample rate/Time scale 
    // see this link -> https://redpitaya.readthedocs.io/en/latest/appsFeatures/examples/acquisition/acqRF-samp-and-dec.html
    rp_AcqSetDecimation(RP_DEC_8);  // Decimaion factor
    rp_AcqSetTriggerLevel(RP_CH_1, 0.5); //trigger level
    rp_AcqSetTriggerDelay(0);
    rp_AcqSetGain(RP_CH_1, RP_LOW); // LV level gain is selected, // user can switch gain using this command

    /* Collect multiple acquisitions */
    for (uint32_t acq = 0; acq < num_acquisitions; acq++) {

        rp_AcqStart();
        sleep(1);  // Wait for fresh samples

        /* After the acquisition is started some time delay is needed to acquire fresh samples into buffer
        Here we have used a time delay of one second but you can calculate the exact value taking into account buffer
        length and sampling rate */

        if (rp_AcqSetTriggerSrc(RP_TRIG_SRC_NOW) != RP_OK) { // instant trigger
            fprintf(stderr, "Trigger source set failed!\n");
            break;
        }

        rp_acq_trig_state_t state = RP_TRIG_STATE_TRIGGERED;
        int timeout = 1000;
        while (timeout--) {
            if (rp_AcqGetTriggerState(&state) != RP_OK || state == RP_TRIG_STATE_TRIGGERED) {
                break;
            }
            usleep(1000); // Small delay to prevent CPU hogging
        }

        bool fillState = false;
        timeout = 1000;
        while (timeout-- && !fillState) {
            if (rp_AcqGetBufferFillState(&fillState) != RP_OK) {
                break;
            }
            usleep(1000); // Small delay to prevent CPU hogging
        }

        uint32_t samples_to_read = buff_size;
        if (acq == num_acquisitions - 1) {  // Last acquisition might be partial
            samples_to_read = total_samples - (acq * buff_size);
        }

        if (rp_AcqGetOldestDataV(RP_CH_1, &samples_to_read, buff) == RP_OK) {
            for (uint32_t i = 0; i < samples_to_read; i++) {
                printf("%f\n", buff[i]);  // Print to screen
                fprintf(file, "%f\n", buff[i]);  // Write to file
            }
        } else {
            fprintf(stderr, "Failed to get data on acquisition %u!\n", acq);
            break;
        }
    }

    /* Cleanup */
    fclose(file);
    free(buff);
    rp_Release();
    return 0;
}