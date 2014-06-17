/*
 * Copyright (c) 2012 Citrix Systems, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 *
 * This is the header file that contains the macros to log error messages to 
 * stderr.
 *  
 *   Sharath George (t_sharathg) Jan 2012
 */

#ifndef _VHD_SYNC_XT_ERROR_LOG_
#define _VHD_SYNC_XT_ERROR_LOG_

/* ---------------- External Header includes ------------------------------- */
#include <stdio.h>
#include <curl/curl.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

/* ---------------- PreProcessor Defines ----------------------------------- */
#define VHD_SYNC_XT_PATH_LENGTH                     256

#define MAX_ERROR_COUNT                              30
#define MAX_ERROR_SIZE                             1024

#define VHD_SYNC_XT_ERRORLOG(...)  	\
    {\
        if (g_error_log_count < MAX_ERROR_COUNT) \
        { \
          snprintf(g_error_log_buffer[g_error_log_count++], \
                   MAX_ERROR_SIZE, \
                   __VA_ARGS__); \
        } \
        else \
        { \
            g_error_log_overflow = true; \
        } \
    };

/* ---------------- Globals ------------------------------------------------ */

extern int g_error_log_count;
extern char g_error_log_buffer[MAX_ERROR_COUNT][MAX_ERROR_SIZE];
extern bool g_error_log_overflow;
 

/* ---------------- Structure Defines -------------------------------------- */

/* ---------------- Function Definitions ----------------------------------- */
void
vhd_sync_xt_error_log_initialize(
    );

void
vhd_sync_xt_error_log_dump(
    );

#endif  // ifndef _VHD_SYNC_XT_ERROR_LOG

