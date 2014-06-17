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
 * This contains the higher level functions of vhdsyncxt that is called
 * by the main function. 
 *
 *
 *   Sharath George (t_sharathg) Jan 2012
 */

/* ---------------- Header includes ---------------------------------------- */

#include <vhdsyncxt.h>

/* ---------------- Function Definitions ----------------------------------- */

bool
vhd_sync_xt_create_config(
    pvhd_sync_xt_config* config
    )
/*
 * This function creates a config struct to store the configuration of 
 * vhdsyncxt.
 *
 * Parameters:
 *
 *      config - Supplies a placeholder to return the config struct
 *          that was created.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */  
{
    bool status;
    pvhd_sync_xt_config config_local;

    status = false;

    config_local = calloc(1, sizeof(vhd_sync_xt_config));
    if (config_local == NULL)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_create_config: Could not allocate memory for config struct.\n");
        status = false;
        goto End;
    }

    *config = config_local;
    config_local = NULL;
    status = true;

End:
    if (config_local != NULL)
    {
        free(config_local);
    }

    return status;
}

void
vhd_sync_xt_destroy_config(
    pvhd_sync_xt_config config
    )
/*
 * This function destroys a config struct that stores the configuration
 * of vhdsyncxt.
 *
 * Parameters:
 *
 *      config - Supplies a poitner to the config struct.
 *
 * Return Value:
 *
 *      None.
 *
 */  
{
    if (config == NULL)
    {
        return;
    }
    
    //
    // If we have the parameters struct, destroy it.
    //
    if (config->parameters != NULL)
    {
        vhd_sync_xt_destroy_parameters(config->parameters);
    }

    //
    // If we have the curl config struct, destroy it.
    //
    if (config->curl_config != NULL)
    {
        vhd_sync_xt_destroy_curl_config(config->curl_config);    
    }

    free(config);
}


int
vhd_sync_xt_process_download(
    pvhd_sync_xt_config config
    )
/*
 * This function processes the download action.
 *
 * Parameters:
 *
 *      config - Supplies a poitner to the config struct.
 *
 *
 * Return Value:
 *
 *      Errno. TBD
 */  
{
    bool status;
    int return_code;

    return_code = 1;

    status = vhd_sync_xt_create_download_context(config->curl_config,
                                                 config->parameters->localpath,
                                                 config->parameters->imageuuid,
                                                 config->parameters->url,
                                                 config->parameters->ca_cert,
                                                 config->parameters->ca_path,
                                                 config->parameters->credentials,
                                                 config->parameters->progress_fd,
                                                 &config->download_context
                                                 );
    if (status == false)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_process_download: Could not create download context.\n");
        return_code = 1;
        goto End;
    }

    return_code = vhd_sync_xt_start_download(config->download_context);
    if (return_code != 0)
    {
        //
        // If file already exists then dont return an error code
        //
        if (return_code == VHD_SYNC_XT_ERROR_FILE_EXISTS)
        {
            return_code = 0;
            goto End;
        }

        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_process_download: Could not process download request/request not complete. Error code : %d\n", return_code);
        goto End;
    }

    status = vhd_sync_xt_finalize_download(config->download_context);
    if (status == false)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_process_download: Could not finalize download.\n");
        return_code = 1;
        goto End;
    }

    return_code = 0;
End:
    return return_code;
}

int
vhd_sync_xt_process_action(
    pvhd_sync_xt_config config
    )
/*
 * This function processes the action that is specified in the config file.
 *
 * Parameters:
 *
 *      config - Supplies a poitner to the config struct.
 *
 *
 * Return Value:
 *
 *      Errno. TBD
 */  
{
    int return_code;

    return_code = 1;
   
    switch (config->parameters->action)
    {
        case ACTION_SHOWHELP:
            vhd_sync_xt_show_help(config->parameters);
            return_code = 0;
            break;

        case ACTION_DOWNLOAD:
            return_code = vhd_sync_xt_process_download(config);
            break;

        default:
            VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_process_action: Action not implemented yet!\n");
            break;
    }

    return return_code;
}
