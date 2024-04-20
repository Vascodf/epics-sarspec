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
#include <string>
#include <sstream>
#include <vector>

#include "sarspec-device.h"

using namespace sarspec_usb;

namespace sarspec_usb_test {
    int runTest(int argc, char* argv[]) {
        int rc;
        return EXIT_SUCCESS;
    }
} // namespace atca_iop_test

int main(int argc, char* argv[])
{
    return sarspec_usb_test::runTest(argc, argv);
}

// vim: syntax=cpp ts=4 sw=4 sts=4 sr et
