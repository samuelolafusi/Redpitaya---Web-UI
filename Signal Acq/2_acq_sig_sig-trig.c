/* Instantly acquiring a signal on a specific channel, signal trigger */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rp.h"

int main(int argc, char **argv) {
    /* Initialize Red Pitaya API */
    if (rp_Init() != RP_OK) {
        fprintf(stderr, "Rp api init failed!\n");
        return 1;
    }

    /* Reset Generation and Acquisition */
    // rp_GenReset();  // Commented out as in original
    rp_AcqReset();

    /* Generation */
    /*LOOB BACK FROM OUTPUT 2 - ONLY FOR TESTING*/
    // rp_GenFreq(RP_CH_1, 20000.0);
    // rp_GenAmp(RP_CH_1, 1.0);
    // rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE);
    // rp_GenOutEnable(RP_CH_1);

    /* Prompt user for custom filename */
    char filename[256];  // Buffer for filename (adjust size as needed)
    printf("Enter the filename to save samples (e.g., samples.txt): ");
    if (scanf("%255s", filename) != 1) {  // Read up to 255 chars, leave room for null terminator
        fprintf(stderr, "Failed to read filename! Using default 'samples.txt'\n");
        strcpy(filename, "samples.txt");  // Fallback to default
    }

    /* Acquisition Setup */
    uint32_t buff_size = 16384;
    float *buff = (float *)malloc(buff_size * sizeof(float));
    if (buff == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        rp_Release();
        return 1;
    }

    // Open file for writing samples with user-specified filename
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to open '%s' for writing!\n", filename);
        free(buff);
        rp_Release();
        return 1;
    }

    // Decimation factor determines sample rate/Time scale 
    // see this link -> https://redpitaya.readthedocs.io/en/latest/appsFeatures/examples/acquisition/acqRF-samp-and-dec.html
    rp_AcqSetDecimation(RP_DEC_8);
    rp_AcqSetTriggerLevel(RP_CH_1, 0.5);    // Trigger level is set in Volts
    rp_AcqSetTriggerDelay(0);

    // There is an option to select coupling when using SIGNALlab 250-12
    // rp_AcqSetAC_DC(RP_CH_1, RP_AC);      // enables AC coupling on Channel 1

    // By default LV level gain is selected
    rp_AcqSetGain(RP_CH_1, RP_LOW);         // user can switch gain using this command

    /* Start Acquisition */
    if (rp_AcqStart() != RP_OK) {
        fprintf(stderr, "Acquisition start failed!\n");
        fclose(file);
        free(buff);
        rp_Release();
        return 1;
    }

    /* After the acquisition is started some time delay is needed to acquire fresh samples into buffer
    Here we have used a time delay of one second but you can calculate the exact value taking into account buffer
    length and sampling rate */
    sleep(1);

    /* Set Trigger Source to Channel 1 Positive Edge */
    // trigger source is input signal
    // trigger on positive edge when trigger level is reached
    rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);  
    rp_acq_trig_state_t state = RP_TRIG_STATE_TRIGGERED;

    int timeout = 10000;  // 10 seconds
    while (timeout--) {
        rp_AcqGetTriggerState(&state);
        if (state == RP_TRIG_STATE_TRIGGERED) {
            break;
        }
        usleep(1000);  // 1 ms delay
    }
    if (state != RP_TRIG_STATE_TRIGGERED) {
        fprintf(stderr, "Trigger timeout!\n");
        fclose(file);
        free(buff);
        rp_Release();
        return 1;
    }

    // !! OS 2.00 or higher only !! //
    bool fillState = false;
    int fill_timeout = 1000;  // 1 second
    while (!fillState && fill_timeout--) {
        rp_AcqGetBufferFillState(&fillState);
        usleep(1000);
    }
    if (!fillState) {
        fprintf(stderr, "Buffer fill timeout!\n");
        fclose(file);
        free(buff);
        rp_Release();
        return 1;
    }

    /* Retrieve and Output Data */
    if (rp_AcqGetOldestDataV(RP_CH_1, &buff_size, buff) != RP_OK) {
        fprintf(stderr, "Failed to get data!\n");
        fclose(file);
        free(buff);
        rp_Release();
        return 1;
    }
    for (int i = 0; i < buff_size; i++) {
        printf("%f\n", buff[i]);          // Print to console
        fprintf(file, "%f\n", buff[i]);   // Write to file
    }

    /* Cleanup */
    fclose(file);
    free(buff);
    rp_Release();
    return 0;
}