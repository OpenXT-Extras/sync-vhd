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
 * This is the header file that contains the main declarations for tying in 
 * the curl library. 
 *
 *
 *   Sharath George (t_sharathg) Jan 2012
 */

#ifndef _VHD_SYNC_XT_CURL_H_
#define _VHD_SYNC_XT_CURL_H_

/* ---------------- External Header includes ------------------------------- */
#include <stdio.h>


/* ---------------- Internal Header includes ------------------------------- */
#include <vhdsyncxt_errorlog.h>

/* ---------------- PreProcessor Defines ----------------------------------- */
#define VHD_SYNC_XT_HTTP_HEADER_REQ_SIZE                100

/* ---------------- Structure Defines -------------------------------------- */
typedef struct _vhd_sync_xt_curl_config
{
    CURL *curlhandle;

    //
    // Connected socket to server.
    //
    int connection_socket;

} vhd_sync_xt_curl_config, *pvhd_sync_xt_curl_config;

/* ---------------- Function Declarations -----------------------------------*/
bool
vhd_sync_xt_create_curl_config(
    pvhd_sync_xt_curl_config* curl_config,
    int connection_socket
    );

void
vhd_sync_xt_destroy_curl_config(
    pvhd_sync_xt_curl_config curl_config
    );

bool
vhd_sync_xt_set_url(
    pvhd_sync_xt_curl_config curl_config,
    char* url,
    char* ca_cert,
    char* ca_path,
    char* credentials
    );

int
vhd_sync_xt_perform_curl(
    pvhd_sync_xt_curl_config curl_config
    );

bool
vhd_sync_xt_set_curl_process_header(
    pvhd_sync_xt_curl_config curl_config,
    void* header_callback,
    void* user_data
    );

bool
vhd_sync_xt_set_curl_get_data(
    pvhd_sync_xt_curl_config curl_config,
    void* header_callback,
    FILE* outfile
    );

bool
vhd_sync_xt_set_curl_data_range(
    pvhd_sync_xt_curl_config curl_config,
    unsigned long int start_offset,
    unsigned long int end_offset
    );

#endif  // ifndef _VHD_SYNC_XT_CURL_H_

