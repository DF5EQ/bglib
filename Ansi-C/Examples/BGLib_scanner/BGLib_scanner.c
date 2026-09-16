// Bluegiga BGLib Ansi-C interface library demonstration sketch
// 2026-09-11 by Peter Bägel (DF5EQ)
// 2013-03-14 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/bglib
//                                                or https://github.com/df5eq/bglib

// Changelog:
//      2026-09-11 - Change from Arduino/C++ to C
//      2013-03-14 - Add support for packet mode
//                   Add support for BLE wake-up
//      2012-11-14 - Initial release

/* ============================================
BGLib Ansi-C interface library code is placed under the MIT license
Copyright (c) 2014 Jeff Rowberg for Arduino/C++
Copyright (c) 2026 Peter Bägel for Ansi-C

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "BGLib.h"
#include "tty.h"

#define DEVICE "/dev/ttyACM0"

static uint8_t isScanActive = 0;

int getch(void) {
    static struct termios oldattr;
    static struct termios newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    memcpy(&newattr,&oldattr,sizeof(struct termios));
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

// ================================================================
// INTERNAL BGLIB CALLBACK FUNCTIONS
// ================================================================

void onBusy() {
    printf("busy\n");
}

void onIdle() {
    /*TODO printf("idle\n")*/;
}

void onTimeout() {
    printf("!!!\tTimeout occurred!\n");
}

void onBeforeTXCommand() {
    printf("TX started\n");
}

void onTXCommandComplete() {
    printf("TX complete\n");
}

// ================================================================
// USER-DEFINED BGLIB RESPONSE CALLBACKS
// ================================================================

void my_rsp_system_hello(void) {
    printf("<--\tsystem_hello\n");
}

void my_rsp_gap_set_scan_parameters(const struct ble_msg_gap_set_scan_parameters_rsp_t* msg) {
    printf("<--\tgap_set_scan_parameters: { result: %x }\n", msg->result);
}

void my_rsp_gap_discover(const struct ble_msg_gap_discover_rsp_t* msg) {
    printf("<--\tgap_discover: { result: %x }\n", msg->result);
}

void my_rsp_gap_end_procedure(const struct ble_msg_gap_end_procedure_rsp_t* msg) {
    printf("<--\tgap_end_procedure: { result: %x }\n", msg->result);
}

// ================================================================
// USER-DEFINED BGLIB EVENT CALLBACKS
// ================================================================

void my_evt_system_boot(const struct ble_msg_system_boot_evt_t* msg) {
    printf("###\tsystem_boot: { ");
    printf("major: %x", msg->major);
    printf(", minor: %x", msg->minor);
    printf(", patch: %x", msg->patch);
    printf(", build: %x", msg->build);
    printf(", ll_version: %x", msg->ll_version);
    printf(", protocol_version: %x", msg->protocol_version);
    printf(", hw: %x", msg->hw);
    printf(" }\n");
}

void my_evt_gap_scan_response(const struct ble_msg_gap_scan_response_evt_t* msg) {
    printf("###\tgap_scan_response: { ");
    printf("rssi: %d", msg-> rssi);
    printf(", packet_type: %x", msg->packet_type);
    printf(", sender: ");
    // this is a "bd_addr" data type, which is a 6-byte uint8_t array
    for (uint8_t i = 0; i < 6; i++) {
        printf("%02x ", msg->sender.addr[i]);
    }
    printf(", address_type: %x", msg->address_type);
    printf(", bond: %x", msg->bond);
    printf(", data: ");
    // this is a "uint8array" data type, which is a length byte and a uint8_t* pointer
    for (uint8_t i = 0; i < msg->data.len; i++) {
        printf("%02x ", msg->data.data[i]);
    }
    printf(" }");
}


/*========== main ===========================================================*/

void main(void) {
/*TODO remove the following test sequence */
    /* switch of buffering for stdin */  
    setvbuf( stdin, NULL, _IONBF , 0);

    /* switch of buffering for stdout */  
    setvbuf( stdout, NULL, _IONBF , 0);

    /* switch of buffering for stderr */  
    setvbuf( stderr, NULL, _IONBF , 0);

    tty_open(DEVICE);

    tty_write(0x00);
    tty_write(0x00);
    tty_write(0x00);
    tty_write(0x01);

    printf("received data: ");

    printf("%02x ", tty_read());
    printf("%02x ", tty_read());
    printf("%02x ", tty_read());
    printf("%02x ", tty_read());
    printf("%02x ", tty_read());
    printf("%02x ", tty_read());
    printf("%02x ", tty_read());
    printf("%02x ", tty_read());

    printf("\n");

    tty_close();
/*TODO end of test sequence */

    BGLib_init(DEVICE, 0);
    printf("BLE112 BGAPI Scanner Demo\n");

    // set up internal status handlers
    // (these are technically optional)
    BGLib_onBusy = onBusy;
    BGLib_onIdle = onIdle;
    BGLib_onTimeout = onTimeout;
    
    // ONLY enable these if you are using the <wakeup_pin> parameter in your firmware's hardware.xml file
    BGLib_onBeforeTXCommand = onBeforeTXCommand;
    BGLib_onTXCommandComplete = onTXCommandComplete;

    // set up BGLib response handlers (called almost immediately after sending commands)
    // (these are also technicaly optional)
    BGLib_ble_rsp_system_hello = my_rsp_system_hello;
    BGLib_ble_rsp_gap_set_scan_parameters = my_rsp_gap_set_scan_parameters;
    BGLib_ble_rsp_gap_discover = my_rsp_gap_discover;
    BGLib_ble_rsp_gap_end_procedure = my_rsp_gap_end_procedure;

    // set up BGLib event handlers (called at unknown times)
    BGLib_ble_evt_system_boot = my_evt_system_boot;
    BGLib_ble_evt_gap_scan_response = my_evt_gap_scan_response;

    while (1) {
        uint8_t ch;

        printf("Operations Menu:\n");
        printf("0) Reset BLE112 module\n");
        printf("1) Say hello to the BLE112 and wait for response\n");
        printf("2) Toggle scanning for advertising BLE devices\n");
        printf("Command?\n");

        // keep polling for new data from BLE
        BGLib_checkActivity(0);
        // check for input from the user
        if (/*TODO Serial.available()*/true) {
            ch = getch();
            switch (ch) {
                case '0':
                    // Reset BLE112 module
                    printf("-->\tsystem_reset: { boot_in_dfu: 0 }\n");
                    BGLib_ble_cmd_system_reset(0);
                    while (BGLib_checkActivity(1000));
                    // system_reset doesn't have a response, but this BGLib
                    // implementation allows the system_boot event specially to
                    // set the "busy" flag to false for this particular case
                    break;
                case '1':
                    // Say hello to the BLE112 and wait for response
                    printf("-->\tsystem_hello\n");
                    BGLib_ble_cmd_system_hello();
                    while (BGLib_checkActivity(1000));
                    // response should come back within milliseconds
                    break;
                case '2':
                    // Toggle scanning for advertising BLE devices
                    if (isScanActive) {
                        isScanActive = 0;
                        printf("-->\tgap_end_procedure\n");
                        BGLib_ble_cmd_gap_end_procedure();
                        while (BGLib_checkActivity(1000));
                        // response should come back within milliseconds
                    } else {
                        isScanActive = 1;
                        printf("-->\tgap_set_scan_parameters: { scan_interval: 0xC8, scan_window: 0xC8, active: 1 }\n");
                        BGLib_ble_cmd_gap_set_scan_parameters(0xC8, 0xC8, 1);
                        while (BGLib_checkActivity(1000));
                        // response should come back within milliseconds
                        printf("-->\tgap_discover: { mode: 2 (GENERIC) }\n");
                        BGLib_ble_cmd_gap_discover(BGLIB_GAP_DISCOVER_GENERIC);
                        while (BGLib_checkActivity(1000));
                        // response should come back within milliseconds
                        // scan response events may happen at any time after this
                    }
                    break;
            }
        }
    }
}

