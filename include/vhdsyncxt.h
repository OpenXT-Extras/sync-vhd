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
 * This is the header file that contains the main declarations for the tool
 * vhdsync. 
 *
 *
 *   Sharath George (t_sharathg) Jan 2012
 */

#ifndef _VHD_SYNC_XT_H_
#define _VHD_SYNC_XT_H_

/* ---------------- External Header includes ------------------------------- */
#include <stdio.h>


/* ---------------- Internal Header includes ------------------------------- */
#include <vhdsyncxt_errorlog.h>
#include <vhdsyncxt_args.h>
#include <vhdsyncxt_curl.h>
#include <vhdsyncxt_download.h>

/* ---------------- PreProcessor Defines ----------------------------------- */

/* ---------------- Structure Defines -------------------------------------- */
typedef struct _vhd_sync_xt_config
{
    //
    // Parameters struct, parsed data from args.
    //
    pvhd_sync_xt_parameters parameters;

    //
    // Curl configuration struct. 
    //
    pvhd_sync_xt_curl_config curl_config;

    //
    // Union of context for different actions.
    //
    union
    {
        pvhd_sync_xt_download_context download_context;
        
    };

} vhd_sync_xt_config, *pvhd_sync_xt_config;

/* ---------------- Function Declarations -----------------------------------*/

bool
vhd_sync_xt_create_config(
    pvhd_sync_xt_config* config
    );

void
vhd_sync_xt_destroy_config(
    pvhd_sync_xt_config config
    );

int
vhd_sync_xt_process_action(
    pvhd_sync_xt_config config
    );


#endif  // ifndef _VHD_SYNC_XT_H_

