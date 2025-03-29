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
CIntParameter FREQUENCY("FREQUENCY", CBaseParameter::RW, 1, 0, 1, 20);
CFloatParameter AMPLITUDE("AMPLITUDE", CBaseParameter::RW, 0.5, 0, 0, 0.5);
CIntParameter WAVEFORM("WAVEFORM", CBaseParameter::RW, 0, 0, 0, 2);
CIntParameter GAIN("GAIN", CBaseParameter::RW, 1, 0, 1, 5);








// Generator config
void set_generator_config()
{
    //Set frequency
    rp_GenFreq(RP_CH_2, FREQUENCY.Value());

    //Set offset
    rp_GenOffset(RP_CH_2, 0.5);

    //Set amplitude
    rp_GenAmp(RP_CH_2, AMPLITUDE.Value());

    //Set waveform
    if (WAVEFORM.Value() == 0)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SINE);
    }
    else if (WAVEFORM.Value() == 1)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_RAMP_UP);
    }
    else if (WAVEFORM.Value() == 2)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SQUARE);
    }
}








const char *rp_app_desc(void)
{
    return (const char *)"WebUI Controlled Function Gen.\n";
}


int rp_app_init(void)
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
    rp_GenOutEnable(RP_CH_2);
    rp_GenResetTrigger(RP_CH_2);
    return 0;
}


int rp_app_exit(void)
{
    fprintf(stderr, "Unloading generator application\n");

    // Disabe generator
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

    //Read data from pin
    rp_AIpinGetValue(0, &val,&raw);

    //Push it to vector
    g_data.erase(g_data.begin());
    g_data.push_back(val * GAIN.Value());

    //Write data to signal
    for(int i = 0; i < SIGNAL_SIZE_DEFAULT; i++)
    {
        VOLTAGE[i] = g_data[i];
    }
}


void UpdateParams(void){}


void OnNewParams(void)
{
    GAIN.Update();

    FREQUENCY.Update();
    AMPLITUDE.Update();
    WAVEFORM.Update();

    // Set generator config
    set_generator_config();
}


void OnNewSignals(void){}


void PostUpdateSignals(void){}