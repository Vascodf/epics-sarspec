/*
 * specAsynPortDriver.h
 *
 * Asyn driver that inherits from the asynPortDriver class to demonstrate its use.
 * It simulates a digital scope looking at a 1kHz 1000-point noisy sine wave.  Controls are
 * provided for time/division, volts/division, volt offset, trigger delay, noise amplitude, update time,
 * and run/stop.
 * Readbacks are provides for the waveform data, min, max and mean values.
 *
 * Author: Mark Rivers
 *
 * Created Feb. 5, 2009
 */

#include "asynPortDriver.h"
#include "sarspec-device.h"

#define NUM_VERT_SELECTIONS 4


/* These are the drvInfo strings that are used to identify the parameters.
 * They are used by asyn clients, including standard asyn device support */
#define P_RunString                "SARSPEC_RUN"                  /* asynInt32,    r/w */
#define P_TriggerDelayString       "SARSPEC_TRIGGER_DELAY"      /* asynInt32,    r/w */
#define P_ExtTriggerString         "SARSPEC_EXTERNAL_TRIGGER"   /* asynInt32,    r/w */
#define P_Coeff0String             "SARSPEC_COEFF_0"            /* asynFloat64,  r/w */
#define P_Coeff1String             "SARSPEC_COEFF_1"            /* asynFloat64,  r/w */
#define P_Coeff2String             "SARSPEC_COEFF_2"            /* asynFloat64,  r/w */
#define P_Coeff3String             "SARSPEC_COEFF_3"            /* asynFloat64,  r/w */
#define P_YDataString              "SARSPEC_YDATA"              /* asynFloat64Array,  r/o */
#define P_XDataString              "SARSPEC_XDATA"              /* asynFloat64Array,  r/o */
#define P_LedString                "SARSPEC_LED"                /* asynInt32,    r/w */
#define P_GainString               "SARSPEC_GAIN"               /* asynInt32,    r/w */
#define P_IntTimeString            "SARSPEC_INT_TIME"           /* asynInt32,    r/w */

/** Class that demonstrates the use of the asynPortDriver base class to greatly simplify the task
  * of writing an asyn port driver.
  * This class does a simple simulation of a digital oscilloscope.  It computes a waveform, computes
  * statistics on the waveform, and does callbacks with the statistics and the waveform data itself.
  * I have made the methods of this class public in order to generate doxygen documentation for them,
  * but they should really all be private. */
class specAsynPortDriver : public asynPortDriver {
public:
    specAsynPortDriver(const char *portName);

    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus readFloat64Array(asynUser *pasynUser, epicsFloat64 *value,
                                        size_t nElements, size_t *nIn);

    /* These are the methods that are new to this class */
    void acquireTask(void);

protected:
    /** Values used for pasynUser->reason, and indexes into the parameter library. */
    int P_Run;
    int P_TriggerDelay;
    int P_ExtTrigger;
    int P_Coeff0;
    int P_Coeff1;
    int P_Coeff2;
    int P_Coeff3;
    int P_YData;
    int P_XData;
    int P_Led;
    int P_Gain;
    int P_IntTime;
private:
    /* Our data */
    epicsEventId eventId_;
    double* pXData_;
    double* pYData_;   
    double pCoeffs_[4];
    
    void setLed();
    void setGain();
    void setIntTime();
    void setCoeffs();
    sarspec_usb::SarspecResDevice specDev;
};
