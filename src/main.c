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
 * This contains the main function of the commandline tool.
 *
 *
 *   Sharath George (t_sharathg) Jan 2012
 */

/* ---------------- Include directives ------------------------------------- */

#include <vhdsyncxt.h>


/* ---------------- Function Definitions ----------------------------------- */

int
main(
    int argc,
    char *argv[]
    )
{
    int return_code;
    pvhd_sync_xt_config config = NULL;

    return_code = 1;
    //
    //  Initialize our error logger.
    //
    vhd_sync_xt_error_log_initialize();

    //
    // Create our global configuration.
    //
    vhd_sync_xt_create_config(&config);

    //
    // Create our parameters struct and parse parameters.
    //
    vhd_sync_xt_create_parameters(&config->parameters);
    vhd_sync_xt_parse_parameters(config->parameters, argc, argv);
    

    //
    // Create our curl configuration struct and initialize curl.
    //
    vhd_sync_xt_create_curl_config(&config->curl_config,
                                   config->parameters->connection_socket);

    //
    // Act based on our commandline parameters.
    //
    return_code = vhd_sync_xt_process_action(config);

    //
    // Destroy our configuration struct. 
    //
    vhd_sync_xt_destroy_config(config);
    config = NULL;

    //
    // Dump our error logs
    //
    vhd_sync_xt_error_log_dump();

End:
    return return_code;
}
