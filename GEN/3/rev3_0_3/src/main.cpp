#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <vector>

#include "main.h"



//Signal size
#define SIGNAL_SIZE_DEFAULT      1024
#define SIGNAL_UPDATE_INTERVAL      1


//Signal
CFloatSignal VOLTAGE("VOLTAGE", SIGNAL_SIZE_DEFAULT, 0.0f);
std::vector<float> g_data(SIGNAL_SIZE_DEFAULT);


// Parameters
//paramter_type parametername_in_c("parameter_name_in_js", CBaseParameter::RW, initial_value, 0, min_value, max_value);
CIntParameter FREQUENCY("FREQUENCY", CBaseParameter::RW, 0, 0, 1, 50000000);
CFloatParameter AMPLITUDE("AMPLITUDE", CBaseParameter::RW, 0.0, 0, 0.1, 1);
CIntParameter WAVEFORM("WAVEFORM", CBaseParameter::RW, 0, 0, 0, 9);
CIntParameter GAIN("GAIN", CBaseParameter::RW, 1, 0, 1, 1);
CIntParameter CHANNEL_STATE_1("CHANNEL_STATE_1", CBaseParameter::RW, 0, 0, 0, 1);

CIntParameter FREQUENCY1("FREQUENCY1", CBaseParameter::RW, 0, 0, 1, 50000000);
CFloatParameter AMPLITUDE1("AMPLITUDE1", CBaseParameter::RW, 0.0, 0, 0.1, 1);
CIntParameter WAVEFORM1("WAVEFORM1", CBaseParameter::RW, 0, 0, 0, 9);
CIntParameter GAIN1("GAIN1", CBaseParameter::RW, 1, 0, 1, 1);
CIntParameter CHANNEL_STATE_2("CHANNEL_STATE_2", CBaseParameter::RW, 0, 0, 0, 1);








// Generator config
void set_generator_config()
{
    //for channel 1, note the names of the parameters, FREQUENCY does not carry 1
    // all parameters for channel 1 does not carry 1

    //Set frequency
    rp_GenFreq(RP_CH_1, FREQUENCY.Value()); // command to set freq on FPGA, rp_GenFreq is a .......... , takes parameter value from JS

    //Set offset
    rp_GenOffset(RP_CH_1, 0.5);

    //Set amplitude
    rp_GenAmp(RP_CH_1, AMPLITUDE.Value());

    //Set waveform
    if (WAVEFORM.Value() == 0) // WAVEFORM value 0 represents SINE WAVE from html, value number must match stated waveform type
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE); //RP_CH_1 and RP_WAVEFORM_SINE are ...... represenst chnannel 1 and the waveform type
    }
    else if (WAVEFORM.Value() == 1)
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SQUARE);
    }
    else if (WAVEFORM.Value() == 2)
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_TRIANGLE);
    }
    else if (WAVEFORM.Value() == 3)
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_RAMP_UP);
    }

    else if (WAVEFORM.Value() == 4)
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_RAMP_DOWN);
    }  

    else if (WAVEFORM.Value() == 5)
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_DC);
    }

    else if (WAVEFORM.Value() == 6)
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_DC_NEG);
    }

    else if (WAVEFORM.Value() == 7)
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_PWM);
    }

    else if (WAVEFORM.Value() == 8)
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_ARBITRARY);
    }
  
    //for channel 2, note the names of the parameters, FREQUENCY carries 1, it is FREQUENCY1, and like that for the rest
    // all parameters for channel 2 carries 1

    //Set frequency
    rp_GenFreq(RP_CH_2, FREQUENCY1.Value());

    //Set offset
    rp_GenOffset(RP_CH_2, 0.5);

    //Set amplitude
    rp_GenAmp(RP_CH_2, AMPLITUDE1.Value());

    //Set waveform
    if (WAVEFORM1.Value() == 0)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SINE);
    }
    else if (WAVEFORM1.Value() == 1)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SQUARE);
    }
    else if (WAVEFORM1.Value() == 2)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_TRIANGLE);
    }

    else if (WAVEFORM1.Value() == 3)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_RAMP_UP);
    }

    else if (WAVEFORM1.Value() == 4)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_RAMP_DOWN);
    }  

    else if (WAVEFORM1.Value() == 5)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_DC);
    }

    else if (WAVEFORM1.Value() == 6)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_DC_NEG);
    }

    else if (WAVEFORM1.Value() == 7)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_PWM);
    }

    else if (WAVEFORM1.Value() == 8)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_ARBITRARY);
    }   
}







// APP description
const char *rp_app_desc(void)
{
    return (const char *)"WebUI Controlled Function Gen.\n";
}


int rp_app_init(void) // rp_app_init is 
{
    fprintf(stderr, "Loading generator application\n");

    // Initialization of API
    if (rpApp_Init() != RP_OK)
    {
        fprintf(stderr, "Red Pitaya API init failed!\n");
        return EXIT_FAILURE;
    }
    else fprintf(stderr, "Red Pitaya API init success!\n");

    //Set signal update interval
    CDataManager::GetInstance()->SetSignalInterval(SIGNAL_UPDATE_INTERVAL);

    // Init generator
    set_generator_config();

    if (CHANNEL_STATE_1.Value() == 1) {
        rp_GenOutEnable(RP_CH_1);
        rp_GenResetTrigger(RP_CH_1);
    } else {
        rp_GenOutDisable(RP_CH_1);
    }

    if (CHANNEL_STATE_2.Value() == 1) {
        rp_GenOutEnable(RP_CH_2);
        rp_GenResetTrigger(RP_CH_2);
    } else {
        rp_GenOutDisable(RP_CH_2);
    }

    return 0;
}


int rp_app_exit(void)
{
    fprintf(stderr, "Unloading generator application\n");

    // Disabe generator
    rp_GenOutDisable(RP_CH_1);
    rp_GenOutDisable(RP_CH_2);

    rpApp_Release();

    return 0;
}


int rp_set_params(rp_app_params_t *p, int len)
{
    return 0;
}


int rp_get_params(rp_app_params_t **p)
{
    return 0;
}


int rp_get_signals(float ***s, int *sig_num, int *sig_len)
{
    return 0;
}








void UpdateSignals(void)
{
    float val;
    uint32_t raw;

    // Check if either channel is enabled
    if (CHANNEL_STATE_1.Value() == 1 || CHANNEL_STATE_2.Value() == 1) 
    {
        rp_AIpinGetValue(0, &val, &raw);

        // Apply gain based on active channels
        float gain_factor = 1.0;
        if (CHANNEL_STATE_1.Value() == 1) {
            gain_factor = GAIN.Value();
        } 
        else if (CHANNEL_STATE_2.Value() == 1) {
            gain_factor = GAIN1.Value();
        }

        g_data.erase(g_data.begin());
        g_data.push_back(val * gain_factor);

        // Write data to signal
        for(int i = 0; i < SIGNAL_SIZE_DEFAULT; i++)
        {
            VOLTAGE[i] = g_data[i];
        }
    }
    else
    {
        // Optionally, you might want to do something when no channel is enabled,
        // like clearing the signal or setting it to a default state.
        // For instance:
        // std::fill(g_data.begin(), g_data.end(), 0.0f); // Reset data
        // Or:
        // for(int i = 0; i < SIGNAL_SIZE_DEFAULT; i++)
        // {
        //     VOLTAGE[i] = 0.0f; // Reset signal to zero
        // }
    }
}



void UpdateParams(void){}


void OnNewParams(void)
{
    GAIN.Update();
    FREQUENCY.Update();
    AMPLITUDE.Update();
    WAVEFORM.Update();
    CHANNEL_STATE_1.Update();

    GAIN1.Update();
    FREQUENCY1.Update();
    AMPLITUDE1.Update();
    WAVEFORM1.Update();
    CHANNEL_STATE_2.Update();

    // Set generator config
    set_generator_config();

    // Dynamically enable/disable channels based on UI toggle
    if (CHANNEL_STATE_1.Value() == 1) {
        rp_GenOutEnable(RP_CH_1);
        rp_GenResetTrigger(RP_CH_1);
    } else {
        rp_GenOutDisable(RP_CH_1);
    }

    if (CHANNEL_STATE_2.Value() == 1) {
        rp_GenOutEnable(RP_CH_2);
        rp_GenResetTrigger(RP_CH_2);
    } else {
        rp_GenOutDisable(RP_CH_2);
    }
}


void OnNewSignals(void){}


void PostUpdateSignals(void){}