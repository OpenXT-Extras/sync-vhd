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
 * This is the file that contains the functions to  log and dump the error
 * messages in the order they occured.
 *
 *   Sharath George (t_sharathg) Jan 2012
 */

/* ---------------- Header includes ---------------------------------------- */

#include <vhdsyncxt_errorlog.h>

/* ---------------- Constant/Global Declarations --------------------------- */

int g_error_log_count = 0;
char g_error_log_buffer[MAX_ERROR_COUNT][MAX_ERROR_SIZE];
bool g_error_log_overflow = false;


/* ---------------- Function Definitions -----------------------------------*/

void
vhd_sync_xt_error_log_initialize(
    )
/*
 * This function initializes the error logger.
 *
 * Parameters:
 *
 *      None.
 *
 * Return Value:
 *
 *      None.
 */  
{
    int i;

    g_error_log_count = 0;
    g_error_log_overflow = false;

    //
    //  Init all the error strings to null.
    //
    for (i=0; i < MAX_ERROR_COUNT; ++i)
    {
        g_error_log_buffer[i][0] = '\0';
    }
}

void
vhd_sync_xt_error_log_dump(
    )
/*
 * This function dumps the error messages to stderr.
 *
 * Parameters:
 *
 *      None.
 *
 * Return Value:
 *
 *      None.
 */  
{
    int i;

    //
    // Print all the error messages from the error stack.
    //
    for (i=0; i < g_error_log_count; ++i)
    {
        fprintf(stderr, "%s", g_error_log_buffer[i]);
    }
}

