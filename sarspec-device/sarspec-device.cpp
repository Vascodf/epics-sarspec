/**
 * Sarspec Linux usb device library
 * Project Name:
 * Design Name:
 * Linux Device Driver
 * working  with kernel 5.10
 *
 * Copyright  2024-present IPFN-Instituto Superior Tecnico, Portugal
 * Creation Date  2024-04-20
 */

#include <stdio.h>
#include <stdlib.h>
#include <libftdi1/ftdi.h>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include "sarspec-device.h"



#ifdef DEBUG
#include <time.h>
#endif

namespace sarspec_usb {

    SarspecResDevice::SarspecResDevice() : ftdi(NULL)
    {
        if ((ftdi = ftdi_new()) == 0)
        {
            fprintf(stderr, "ftdi_new failed\n");
        }
        ftdi_set_latency_timer(ftdi, 1);
        ftdi -> usb_read_timeout = 300000;
        ftdi -> usb_write_timeout = 300000;
    }
    SarspecResDevice::~SarspecResDevice()
    {
    }

} // namespace sarspec_usb
// vim: syntax=cpp ts=4 sw=4 sts=4 sr et

