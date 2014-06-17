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
 * This is the header file that contains the declarations for 
 * functions that handle the argumets from the commandline.
 *
 *   Sharath George (t_sharathg) Jan 2012
 */

#ifndef _VHD_SYNC_XT_ARGS_H
#define _VHD_SYNC_XT_ARGS_H

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
#include <libgen.h>

/* ---------------- Internal Header includes ------------------------------- */

#include <vhdsyncxt_errorlog.h>

/* ---------------- PreProcessor Defines ----------------------------------- */

#define VHD_SYNC_XT_ARG_STRING_LENGTH       128
#define VHD_SYNC_XT_DEFAULT_NAME            "vhd-syncxt"
#define VHD_SYNC_XT_DEFAULT_PATH            "."

/* ---------------- Constant/Global Declarations --------------------------- */

/* ---------------- Structure Defines -------------------------------------- */

typedef enum _vhd_sync_xt_parameters_action
{
    ACTION_MINIMUM = 0,
    ACTION_SHOWHELP = 0,
    ACTION_DOWNLOAD,
    ACTION_UPLOAD,
    ACTION_UPGRADE,
    ACTION_BACKUP,
    ACTION_MAXIMUM
} vhd_sync_xt_parameters_action, *pvhd_sync_xt_parameters_action;

typedef struct _vhd_sync_xt_parameters
{
    vhd_sync_xt_parameters_action action;

    char *url;
    char *imageuuid;
    char *localpath;
    char *ca_cert;
    char *ca_path;
    char *credentials;


    //
    // File descriptor to send progress status to.
    //
    int progress_fd;

    //
    // File descriptor of connected socket to server.
    //
    int connection_socket;

    //
    // cache commandline params.
    //
    int argc;
    char **argv;

} vhd_sync_xt_parameters, *pvhd_sync_xt_parameters;

/* ---------------- Function Declarations -----------------------------------*/

bool
vhd_sync_xt_create_parameters(
    pvhd_sync_xt_parameters* parameters
    );

bool
vhd_sync_xt_parse_parameters(
    pvhd_sync_xt_parameters parameters,
    int argc, 
    char *argv[]
    );

void
vhd_sync_xt_destroy_parameters(
    pvhd_sync_xt_parameters parameters
    );

void 
vhd_sync_xt_show_help(
    pvhd_sync_xt_parameters parameters
    );

#endif   // _VHD_SYNC_XT_ARGS_H
