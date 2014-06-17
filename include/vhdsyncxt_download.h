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
 * This is the header file that contains the main declarations for the 
 * download action.
 * 
 *      Sharath George (t_sharathg) Jan 2012
 */

#ifndef _VHD_SYNC_XT_DOWNLOAD_H_
#define _VHD_SYNC_XT_DOWNLOAD_H_

/* ---------------- External Header includes ------------------------------- */
#include <stdio.h>
#include <string.h>

/* ---------------- Internal Header includes ------------------------------- */
#include <vhdsyncxt_errorlog.h>
#include <vhdsyncxt_curl.h>

/* ---------------- PreProcessor Defines ----------------------------------- */

#define VHD_SYNC_XT_PARTIAL_FILE_EXTENSION              ".part"
#define VHD_SYNC_XT_DEFAULT_CHUNK_SIZE                  10000000
#define VHD_SYNC_XT_PROGRESS_MESSAGE_LENGTH             100


#define VHD_SYNC_XT_ERROR_FILE_EXISTS                   1000

/* ---------------- Structure Defines -------------------------------------- */
typedef struct _vhd_sync_xt_download_context
{
    pvhd_sync_xt_curl_config    curl_config; // not owned by this module
    
    //
    // File pointer we are writing out data to.
    //
    FILE *out;

    //
    // Offset to download from for partial downloads.
    //
    unsigned long int start_offset;
    unsigned long int file_size;
    unsigned long int chunk_size;

    //
    // Current state of our download.
    //
    unsigned long int current_offset;

    //
    // pointers to download parameters.
    //
    char         *local_path;
    char         *local_filename;
    char         *url;
    char         *ca_cert;
    char         *ca_path;
    char         *credentials;

    char local_file_path[VHD_SYNC_XT_PATH_LENGTH];
    char partial_file_path[VHD_SYNC_XT_PATH_LENGTH];

    //
    // Progress monitoring.
    //
    int progress_fd;
} vhd_sync_xt_download_context, *pvhd_sync_xt_download_context;

/* ---------------- Function Declarations -----------------------------------*/
bool
vhd_sync_xt_create_download_context(
    pvhd_sync_xt_curl_config curl_config,
    char* local_path,
    char* local_filename,
    char* url,
    char* ca_cert,
    char* ca_path,
    char* credentials,
    int progress_fd,
    pvhd_sync_xt_download_context* download_context
    );

void
vhd_sync_xt_destroy_download_context(
    pvhd_sync_xt_download_context download_context
    );

bool
vhd_sync_xt_exists_partial_file(
    pvhd_sync_xt_download_context download_context
    );

int
vhd_sync_xt_start_download(
    pvhd_sync_xt_download_context download_context
    );

bool
vhd_sync_xt_finalize_download(
    pvhd_sync_xt_download_context download_context
    );

#endif  // ifndef _VHD_SYNC_XT_DOWNLOAD_H_

