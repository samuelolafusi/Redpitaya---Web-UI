#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <vector>

#include "main.h"

// lots of stuffs here are stated in the redpitaya ecosystem files compatible with my OS version .....
// ...... Version - RedPitaya_OS_2.05-37
// example codes in Redpitaya_Ecosystem\RedPitaya-master\Examples also gave insights
// check redpitaya github for the ecosystem version for RedPitaya_OS_2.05-37
// see files Redpitaya_Ecosystem\RedPitaya-master\rp-api\api\include and also ..\src

//Signal size
#define SIGNAL_SIZE_DEFAULT      1024
#define SIGNAL_UPDATE_INTERVAL 10 // 10ms updates
#define SAMPLE_RATE 1000.0f       // 1 kHz for plotting (adjustable)


//Signal
CFloatSignal VOLTAGE_CH1("VOLTAGE_CH1", SIGNAL_SIZE_DEFAULT, 0.0f);
CFloatSignal VOLTAGE_CH2("VOLTAGE_CH2", SIGNAL_SIZE_DEFAULT, 0.0f);
std::vector<float> g_data_ch1(SIGNAL_SIZE_DEFAULT);
std::vector<float> g_data_ch2(SIGNAL_SIZE_DEFAULT);


// paramter_type parametername_in_c("parameter_name_in_js", CBaseParameter::RW, initial_value, 0, min_value, max_value);
// initial value, min value, max value must correspond to html and Redpitaya device capabilities
// note, max output voltage of baord is -1V/+1V, total (offset + amplitude) will be clipped if above the limit.

// Parameters for channel 1
CIntParameter FREQUENCY("FREQUENCY", CBaseParameter::RW, 0, 0, 0, 50000000);
CFloatParameter AMPLITUDE("AMPLITUDE", CBaseParameter::RW, 0.0, 0, 0.0, 1.0);
CIntParameter WAVEFORM("WAVEFORM", CBaseParameter::RW, 0, 0, 0, 5);
CIntParameter PHASE("PHASE", CBaseParameter::RW, 0, 0, 0, 360);
CFloatParameter OFFSET("OFFSET", CBaseParameter::RW, 0.0, 0, -1, 1);
CIntParameter CHANNEL_STATE_1("CHANNEL_STATE_1", CBaseParameter::RW, 0, 0, 0, 1);

CIntParameter GAIN("GAIN", CBaseParameter::RW, 0, 0, 0, 5);

// Parameters for channel 2
CIntParameter FREQUENCY1("FREQUENCY1", CBaseParameter::RW, 0, 0, 0, 50000000);
CFloatParameter AMPLITUDE1("AMPLITUDE1", CBaseParameter::RW, 0.0, 0, 0.0, 1.0);
CIntParameter WAVEFORM1("WAVEFORM1", CBaseParameter::RW, 0, 0, 0, 5);
CIntParameter PHASE1("PHASE1", CBaseParameter::RW, 0, 0, 0, 360);
CFloatParameter OFFSET1("OFFSET1", CBaseParameter::RW, 0.0, 0, -1, 1); 
CIntParameter CHANNEL_STATE_2("CHANNEL_STATE_2", CBaseParameter::RW, 0, 0, 0, 1);



// Generator config
void set_generator_config()
{
    // These formats/keyword/structure depending on what you want to create, are specified in RedPitaya-master\rp-api\api\include and also ..\src ......
    // ........ in the ecosystem for RedPitaya_OS_2.05-37 on Github
    // note the names of the local parameters for both channels in JS as this is the same name for parameters here in CPP

    rp_GenFreq(RP_CH_1, FREQUENCY.Value());
    rp_GenOffset(RP_CH_1, OFFSET.Value());
    rp_GenAmp(RP_CH_1, AMPLITUDE.Value());
    rp_GenPhase(RP_CH_1, PHASE.Value());

    //Set waveform
    // WAVEFORM value 0 represents SINE WAVE from html, value number must match stated waveform type

    switch (WAVEFORM.Value()) {
        case 0: rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE); break;
        case 1: rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SQUARE); break;
        case 2: rp_GenWaveform(RP_CH_1, RP_WAVEFORM_TRIANGLE); break;
        case 3: rp_GenWaveform(RP_CH_1, RP_WAVEFORM_RAMP_UP); break;
        case 4: rp_GenWaveform(RP_CH_1, RP_WAVEFORM_RAMP_DOWN); break;
    }


    rp_GenFreq(RP_CH_2, FREQUENCY1.Value());
    rp_GenOffset(RP_CH_2, OFFSET1.Value());
    rp_GenAmp(RP_CH_2, AMPLITUDE1.Value());
    rp_GenPhase(RP_CH_2, PHASE1.Value());
    switch (WAVEFORM1.Value()) {
        case 0: rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SINE); break;
        case 1: rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SQUARE); break;
        case 2: rp_GenWaveform(RP_CH_2, RP_WAVEFORM_TRIANGLE); break;
        case 3: rp_GenWaveform(RP_CH_2, RP_WAVEFORM_RAMP_UP); break;
        case 4: rp_GenWaveform(RP_CH_2, RP_WAVEFORM_RAMP_DOWN); break;
    }
}


// APP description
const char *rp_app_desc(void)
{
    return (const char *)"WebUI Controlled Function Gen.\n";
}

//APP initialization
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

    for (int i = 0; i < SIGNAL_SIZE_DEFAULT; i++) {
        g_data_ch1[i] = OFFSET.Value();
        g_data_ch2[i] = OFFSET1.Value();
        VOLTAGE_CH1[i] = g_data_ch1[i];
        VOLTAGE_CH2[i] = g_data_ch2[i];
    }

    //Set signal update interval
    CDataManager::GetInstance()->SetSignalInterval(SIGNAL_UPDATE_INTERVAL);

    // Init generator
    // Here, the toggle state from JS determines the activation of channels

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


int rp_get_signals(float ***s, int *sig_num, int *sig_len) {

    *sig_num = CHANNEL_STATE_1.Value() + CHANNEL_STATE_2.Value();
    if (*sig_num == 0) {
        *sig_num = 1; // Send at least one signal to avoid crash
        *sig_len = SIGNAL_SIZE_DEFAULT;
        *s = (float **)malloc(*sig_num * sizeof(float *));
        (*s)[0] = (float *)malloc(*sig_len * sizeof(float));
        for (int i = 0; i < *sig_len; i++) (*s)[0][i] = 0.0f;
        return 0;
    }

    *sig_len = SIGNAL_SIZE_DEFAULT;
    *s = (float **)malloc(*sig_num * sizeof(float *));
    int idx = 0;
    if (CHANNEL_STATE_1.Value()) {
        (*s)[idx] = (float *)malloc(*sig_len * sizeof(float));
        for (int i = 0; i < *sig_len; i++) (*s)[idx][i] = VOLTAGE_CH1[i];
        idx++;
    }
    if (CHANNEL_STATE_2.Value()) {
        (*s)[idx] = (float *)malloc(*sig_len * sizeof(float));
        for (int i = 0; i < *sig_len; i++) (*s)[idx][i] = VOLTAGE_CH2[i];
    }
    return 0;
}



void UpdateSignals(void) {
    static float phase1 = PHASE.Value();
    static float phase2 = PHASE1.Value();
    float dt = SIGNAL_UPDATE_INTERVAL / 1000.0f;
    int samples_per_update = (int)(SAMPLE_RATE * dt + 0.5f);

    if (CHANNEL_STATE_1.Value()) {
        float freq = FREQUENCY.Value();
        float amp = AMPLITUDE.Value() * (GAIN.Value() + 1); // +1 to avoid 0 gain
        int wave = WAVEFORM.Value();
        float offset = OFFSET.Value();
        for (int i = 0; i < samples_per_update; i++) {
            float t = phase1 / (freq > 0 ? freq : 1); // Avoid div by 0
            float val;
            switch (wave) {
                case 0: val = offset + amp * sinf(2 * M_PI * freq * t); break;
                case 1: val = offset + amp * (sinf(2 * M_PI * freq * t) > 0 ? 1 : -1); break;
                case 2: case 3: case 4: {
                    float period = fmodf(freq * t, 1.0);
                    val = offset + amp * (2 * period - 1);
                    break;
                }
                default: val = offset; // Simplified for others
            }
            g_data_ch1.erase(g_data_ch1.begin());
            g_data_ch1.push_back(val);
            phase1 += freq * dt / samples_per_update;
        }
        for (int i = 0; i < SIGNAL_SIZE_DEFAULT; i++) VOLTAGE_CH1[i] = g_data_ch1[i];
    }

    if (CHANNEL_STATE_2.Value()) {
        float freq = FREQUENCY1.Value();
        float amp = AMPLITUDE1.Value() * (GAIN.Value() + 1);
        int wave = WAVEFORM1.Value();
        float offset = OFFSET1.Value();
        for (int i = 0; i < samples_per_update; i++) {
            float t = phase2 / (freq > 0 ? freq : 1);
            float val;
            switch (wave) {
                case 0: val = offset + amp * sinf(2 * M_PI * freq * t); break;
                case 1: val = offset + amp * (sinf(2 * M_PI * freq * t) > 0 ? 1 : -1); break;
                case 2: case 3: case 4: {
                    float period = fmodf(freq * t, 1.0);
                    val = offset + amp * (2 * period - 1);
                    break;
                }
                default: val = offset;
            }
            g_data_ch2.erase(g_data_ch2.begin());
            g_data_ch2.push_back(val);
            phase2 += freq * dt / samples_per_update;
        }
        for (int i = 0; i < SIGNAL_SIZE_DEFAULT; i++) VOLTAGE_CH2[i] = g_data_ch2[i];
    }
}



void UpdateParams(void){}


void OnNewParams(void)
{   // this part updates the cpp paramters based on new inputs from user

    GAIN.Update();
    
    FREQUENCY.Update();
    AMPLITUDE.Update();
    PHASE.Update();
    WAVEFORM.Update();
    OFFSET.Update();
    CHANNEL_STATE_1.Update();

    
    FREQUENCY1.Update();
    AMPLITUDE1.Update();
    PHASE1.Update();
    WAVEFORM1.Update();
    OFFSET1.Update();
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


void OnNewSignals(void) {
    
    // VOLTAGE.Update(); // Mark signal as updated
}

void PostUpdateSignals(void){}