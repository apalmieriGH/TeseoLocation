/**
*******************************************************************************
* @file    main.cpp
* @author  AST / Central Lab
* @version V1.0.0
* @date    June-2017
* @brief   Teseo Location Hello World
*
*******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
********************************************************************************
*/

#include "mbed.h"
#include "Teseo.h"
#include "GPSProvider.h"

#define DEBUG_RX_PIN   D0
#define DEBUG_TX_PIN   D1

/* appliation commands */
typedef enum AppCmd {
    APP_CMD_IDLE,           // No special command
    APP_CMD_HELP,           // Show the supported commands
    APP_CMD_START,          // Start location
    APP_CMD_STOP,           // Stop location
    APP_CMD_GETLASTLOC,     // Get last location
    APP_CMD_RESET,          // Debug command, pull reset pin high level
    APP_CMD_GET_DEVICE_INFO // Get Device Info
} eAppCmd;

static void _AppShowCmd(void);
static void _ConsoleRxHandler(void);
static void _AppCmdProcess(char *pCmd);
static void _AppShowLastPosition(const GPSProvider::LocationUpdateParams_t *lastLoc);

static int sAppCmd  = APP_CMD_IDLE;

Serial serialDebug(DEBUG_TX_PIN, DEBUG_RX_PIN);
#define TESEO_APP_LOG_INFO(...)   serialDebug.printf(__VA_ARGS__)

static GPSProvider gnss;

void
locationHandler(const GPSProvider::LocationUpdateParams_t *params)
{
  if (params->valid) {
    /* application specific handling of location data; */
    TESEO_APP_LOG_INFO("locationHandler...\r\n");
  }
}

static void
_ConsoleRxHandler(void)
{
    static char cmd[32] = {0};
    char        ch;

    ch = serialDebug.getc();
    serialDebug.putc(ch);
    if (ch == '\r') {
        serialDebug.putc('\n');
        if (strlen(cmd) > 0) {
            _AppCmdProcess(cmd);
            memset(cmd, 0, sizeof(cmd));
        }
    } else {
        cmd[strlen(cmd)] = ch;
    }
}

int main() {

  serialDebug.format(8, Serial::None, 1);
  serialDebug.baud(115200);
  serialDebug.attach(&_ConsoleRxHandler);
  
  gnss.reset();
  gnss.onLocationUpdate(locationHandler);
  TESEO_APP_LOG_INFO("Success to new GNSS.\r\n");
  
  _AppShowCmd();
        
  while (true) {
    switch (sAppCmd) {
    case APP_CMD_HELP:
      sAppCmd = APP_CMD_IDLE;
      _AppShowCmd();
      break;
    case APP_CMD_IDLE:
      gnss.process();
      break;
    case APP_CMD_START:
      sAppCmd = APP_CMD_IDLE;
      TESEO_APP_LOG_INFO("start gnss.\r\n");
      gnss.start();
      break;
    case APP_CMD_STOP:
      sAppCmd = APP_CMD_IDLE;
      TESEO_APP_LOG_INFO("stop gnss.\r\n");
      gnss.stop();
      break;
    case APP_CMD_RESET:
      sAppCmd = APP_CMD_IDLE;
      gnss.reset();
      TESEO_APP_LOG_INFO("reset on.\r\n");
      break;
    case APP_CMD_GETLASTLOC:
      TESEO_APP_LOG_INFO("get last loc.\r\n");
      sAppCmd = APP_CMD_IDLE;
      _AppShowLastPosition(gnss.getLastLocation());
      break;
    case APP_CMD_GET_DEVICE_INFO:
      TESEO_APP_LOG_INFO("get device info.\r\n");
      sAppCmd = APP_CMD_IDLE;
      if(gnss.haveDeviceInfo()) {
        TESEO_APP_LOG_INFO(gnss.getDeviceInfo());
      }
      TESEO_APP_LOG_INFO("\r\n");
      break;
    }
  }
}

static void
_AppShowLastPosition(const GPSProvider::LocationUpdateParams_t *lastLoc)
{
  char msg[256];
  GPSProvider::LocationUpdateParams_t lastLocation = *lastLoc;

  if(lastLocation.valid == true) {
    sprintf(msg,"Latitude:\t\t[ %.0f' %d'' ]\n\r",
            (lastLocation.lat - ((int)lastLocation.lat % 100)) / 100, 
            ((int)lastLocation.lat % 100));          
    TESEO_APP_LOG_INFO(msg);
    
    sprintf(msg,"Longitude:\t\t[ %.0f' %d'' ]\n\r",
            (lastLocation.lon - ((int)lastLocation.lon % 100)) / 100, 
            ((int)lastLocation.lon % 100));          
    TESEO_APP_LOG_INFO(msg);
    
    sprintf(msg,"Altitude:\t\t[ %.2f ]\n\r",
            lastLocation.altitude);
    TESEO_APP_LOG_INFO(msg);
    
    sprintf(msg,"Satellites locked:\t[ %d ]\n\r",
            lastLocation.numGPSSVs);
    TESEO_APP_LOG_INFO(msg);
    
    sprintf(msg, "UTC:\t\t\t[ %d ]\n\r",
            (int)lastLocation.utcTime);
    TESEO_APP_LOG_INFO(msg);
    
  } else {
    sprintf(msg, "Last position wasn't valid.\n\n\r");
    TESEO_APP_LOG_INFO(msg);
  }
}
    
static void
_AppShowCmd(void)
{
    TESEO_APP_LOG_INFO("Location commands:\r\n");
    TESEO_APP_LOG_INFO("    help         - help to show supported commands\r\n");
    TESEO_APP_LOG_INFO("    start        - begin location\r\n");
    TESEO_APP_LOG_INFO("    stop         - end location\r\n");
    TESEO_APP_LOG_INFO("    getlastloc   - get last location\r\n");
    TESEO_APP_LOG_INFO("    getdevinfo   - get device info\r\n");
}

static void
_AppCmdProcess(char *pCmd)
{
    if (strcmp(pCmd, "help") == 0) {
        sAppCmd = APP_CMD_HELP;
    } else if (strcmp(pCmd, "start") == 0) {
        sAppCmd = APP_CMD_START;
    } else if (strcmp(pCmd, "stop") == 0) {
        sAppCmd = APP_CMD_STOP;
    } else if (strcmp(pCmd, "getlastloc") == 0) {
        sAppCmd = APP_CMD_GETLASTLOC;
    } else if (strcmp(pCmd, "reset") == 0) {
        sAppCmd = APP_CMD_RESET;
    } else if (strcmp(pCmd, "getdevinfo") == 0) {
        sAppCmd = APP_CMD_GET_DEVICE_INFO;
    } else {
        TESEO_APP_LOG_INFO("\r\nUnknown command %s\r\n", pCmd);
    }

    TESEO_APP_LOG_INFO("\r\n");
}
