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
 * This file contains wrapper functions that call into the curl library
 * and maintain any state associated with our curl calls. All calls into curl
 * go through here. 
 *
 *
 *   Sharath George (t_sharathg) Jan 2012
 */

/* ---------------- Header includes ---------------------------------------- */

#include <vhdsyncxt_curl.h>

/* ---------------- Function Definitions ----------------------------------- */


bool
vhd_sync_xt_create_curl_config(
    pvhd_sync_xt_curl_config* curl_config,
    int connection_socket
    )
/*
 * This function creates a curl configuration struct to store our curl 
 * configuration.
 *
 * Parameters:
 *
 *      curl_config - Supplies a placeholder to return the curl configuration
 *          struct that was created.
 *          
 *      connection_socket - Supplies a an opened socket that is already connected
 *          to the server that should be used by curl.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */  

{
    bool status;
    pvhd_sync_xt_curl_config    curl_config_local;

    status = false;
    curl_config_local = NULL;

    curl_config_local = calloc(1, sizeof(vhd_sync_xt_curl_config));
    if (curl_config_local == NULL)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_create_curl_config: Could not allocate memory for curl configuration.\n");
        status = false;
        goto End;
    }

    curl_config_local->curlhandle  = curl_easy_init();
    if (curl_config_local->curlhandle == NULL)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_create_curl_config: Could not init curl library.\n");
        status = false;
        goto End;
    }

    curl_config_local->connection_socket = connection_socket;

    *curl_config = curl_config_local;
    curl_config_local = NULL;
    status = true;   

End:
    if (curl_config_local != NULL)
    {
        vhd_sync_xt_destroy_curl_config(curl_config_local);
    }

    return status;
    
}

void
vhd_sync_xt_destroy_curl_config(
    pvhd_sync_xt_curl_config curl_config
    )
/*
 * This function destroys a curl_config struct that stores the curl
 * configuration.
 *
 * Parameters:
 *
 *      curl_config - Supplies a poitner to the curl configuration.
 *
 * Return Value:
 *
 *      None.
 */
{
    if (curl_config == NULL)
    {
        return;
    }

    if (curl_config->curlhandle)
    {
        curl_easy_cleanup(curl_config->curlhandle);
    }

    free(curl_config);
}

curl_socket_t
vhd_sync_xt_curl_opensocket (
        void *user_data,
        curlsocktype purpose,
        struct curl_sockaddr *address
        )
/*
 * This function is set as the callback to open a socket from curl, if we
 * already have socket fd.
 *
 * Parameters:
 *
 *      user_data - Supplies a pointer to our curl config struct.
 *
 *      purpose - Curl supplies a purpose for this socket call.
 *
 *      address - Curl supplies an address for this socket
 *
 * Return Value:
 *
 *      Returns the sockfd we have already opened.
 */
{
    pvhd_sync_xt_curl_config curl_config;

    curl_config  = (pvhd_sync_xt_curl_config)user_data;

    return curl_config->connection_socket;
}

int
vhd_sync_xt_curl_sockopt_callback(
        void *user_data,
        curl_socket_t sockfd,
        curlsocktype purpose
        )
/*
 * This function is set as the callback after the socket call from curl.
 *
 * Parameters:
 *
 *      user_data - Supplies a pointer to our curl config struct.
 *
 *      sockfd - Supplies the socket fd for which this is called.
 *
 *      prupose - Curl supplies a purpose for this call.
 *
 * Return Value:
 *
 *      Returns CURL_SOCKOPT_ALREADY_CONNECTED to indicate that our
 *      socket is already connected.
 */
{
    return CURL_SOCKOPT_ALREADY_CONNECTED;
}


bool
vhd_sync_xt_set_url(
    pvhd_sync_xt_curl_config curl_config,
    char* url,
    char* ca_cert,
    char* ca_path,
    char* credentials
    )
/*
 * This function sets the url for curl config.
 *
 * Parameters:
 *
 *      curl_config - Supplies a pointer to the curl configuration.
 *
 *      url - specifies the url for curl
 *
 *      ca_cert - Supplies a certificate for the server.
 *
 *      ca_path - Supplies a ca path to verify the server certificate.
 *
 *      credentials - Supplies login credentials for auth with the server.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */  
{
    CURLcode res;
    bool status;

    status = false;

    res = curl_easy_setopt(curl_config->curlhandle, CURLOPT_URL, url);
    if (res != CURLE_OK)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_url: Could not set url option.\n");
        status = false;
        goto End;
    }


    //
    // If we already have a socket fd through which to channel
    // our data, then instruct curl to use it.
    //
    if (curl_config->connection_socket != 0)
    {
        res = curl_easy_setopt(curl_config->curlhandle,
                               CURLOPT_OPENSOCKETDATA,
                               curl_config
                               );
        if (res != CURLE_OK)
        {
            VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_url: Could not set opensocketdata option.\n");
            status = false;
            goto End;
        }

        res = curl_easy_setopt(curl_config->curlhandle,
                               CURLOPT_OPENSOCKETFUNCTION,
                               vhd_sync_xt_curl_opensocket
                               );
        if (res != CURLE_OK)
        {
            VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_url: Could not set opensocketfunction option.\n");
            status = false;
            goto End;
        }

        res = curl_easy_setopt(curl_config->curlhandle,
                               CURLOPT_SOCKOPTDATA,
                               curl_config
                               );
        if (res != CURLE_OK)
        {
            VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_url: Could not set sockoptdata option.\n");
            status = false;
            goto End;
        }

        res = curl_easy_setopt(curl_config->curlhandle,
                                CURLOPT_SOCKOPTFUNCTION,
                                vhd_sync_xt_curl_sockopt_callback);
        if (res != CURLE_OK)
        {
            VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_url: Could not set sockoptfunction option.\n");
            status = false;
            goto End;
        }

        //
        // This means that the client is already connected  go to end.
        //
        status = true;
        goto End;
    }

    //
    // If we have a  certificate for the server verify it.
    //
    if (ca_cert != NULL)
    {
        res = curl_easy_setopt(curl_config->curlhandle,
                               CURLOPT_CAINFO,
                               ca_cert
                               );
        if (res != CURLE_OK)
        {
            VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_url: Could not set cert info option.\n");
            status = false;
            goto End;
        }
    }

    //
    // If we have a ca path to verify the server cert then verify the server cert.
    //
    if (ca_path != NULL)
    {
        res = curl_easy_setopt(curl_config->curlhandle,
                               CURLOPT_CAPATH,
                               ca_path
                               );
        if (res != CURLE_OK)
        {
            VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_url: Could not set cert path option.\n");
            status = false;
            goto End;
        }

    }

    //
    // If we have auth credentials set them.
    //
    if (credentials != NULL)
    {
        res = curl_easy_setopt(curl_config->curlhandle,
                               CURLOPT_USERPWD,
                               credentials
                               );
        if (res != CURLE_OK)
        {
            VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_url: Could not set user credential option.\n");
            status = false;
            goto End;
        }

        res = curl_easy_setopt(curl_config->curlhandle,
                               CURLOPT_HTTPAUTH,
                               CURLAUTH_DIGEST
                               );
        if (res != CURLE_OK)
        {
            VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_url: Could not set auth type.\n");
            status = false;
            goto End;
        }
    }

    status = true;

End:
    return status;
}


int
vhd_sync_xt_perform_curl(
    pvhd_sync_xt_curl_config curl_config
    )
/*
 * This function peforms the curl operation that has been configured.
 *
 * Parameters:
 *
 *      curl_config - Supplies a poitner to the curl configuration.
 *
 * Return Value:
 *
 *      Errno. TBD.
 */ 
{
    return curl_easy_perform(curl_config->curlhandle);
}


bool
vhd_sync_xt_set_curl_process_header(
    pvhd_sync_xt_curl_config curl_config,
    void* header_callback,
    void* user_data
    )
/*
 * This function sets the curl option to send callbacks to the requested
 * callback function for the header of the URL
 *
 * Parameters:
 *
 *      curl_config - Supplies a poitner to the curl configuration.
 *
 *      header_callback - Suppplies a callback for the header of the url.
 *
 *      user_data - Supplies a user data context that is passed to the
 *          callback.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */
{
    bool status;
    CURLcode res;
    
    status = false;
    res = curl_easy_setopt(curl_config->curlhandle,
                           CURLOPT_NOBODY,
                           1
                           );
    if (res != CURLE_OK)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_curl_process_header: Could not set nobody option.\n");
        status = false;
        goto End;
    }

    res = curl_easy_setopt(curl_config->curlhandle,
                           CURLOPT_HEADERFUNCTION,
                           header_callback
                           );
    if (res != CURLE_OK)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_curl_process_header: Could not set nobody option.\n");
        status = false;
        goto End;
    }

    res = curl_easy_setopt(curl_config->curlhandle,
                           CURLOPT_WRITEHEADER,
                           user_data
                           );
    if (res != CURLE_OK)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_curl_process_header: Could not set nobody option.\n");
        status = false;
        goto End;
    }
    status = true;

End:
    return status;
}


bool
vhd_sync_xt_set_curl_get_data(
    pvhd_sync_xt_curl_config curl_config,
    void* header_callback,
    FILE* outfile
    )
/*
 * This function sets the curl option to send callbacks to the requested
 * callback function for the header of the URL
 *
 * Parameters:
 *
 *      curl_config - Supplies a poitner to the curl configuration.
 *
 *      header_callback - Suppplies a callback for the header of the url.
 *
 *      outfile - Supplies a file stream to write the data to.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */
{
    bool status;
    CURLcode res;

    status = false;

    //
    // Set the option to get the body.
    //
    res = curl_easy_setopt(curl_config->curlhandle,
                           CURLOPT_NOBODY,
                           0
                           );
    if (res != CURLE_OK)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_curl_get_data: Could not set nobody option.\n");
        status = false;
        goto End;
    }
    //
    // Do nothing in the header callback.
    //
    res = curl_easy_setopt(curl_config->curlhandle,
                           CURLOPT_HEADERFUNCTION,
                           header_callback
                           );
    if (res != CURLE_OK)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_curl_get_data: Could not set header function.\n");
        status = false;
        goto End;
    }

    res = curl_easy_setopt(curl_config->curlhandle,
                           CURLOPT_FILE,
                           outfile
                           );
    if (res != CURLE_OK)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_curl_get_data: Could not set FILE option.\n");
        status = false;
        goto End;
    }

    status = true;

End:
    return status;
}


bool
vhd_sync_xt_set_curl_data_range(
    pvhd_sync_xt_curl_config curl_config,
    unsigned long int start_offset,
    unsigned long int end_offset
    )
/*
 * This function sets the curl range option to get a range of bytes.
 *
 * Parameters:
 *
 *      curl_config - Supplies a poitner to the curl configuration.
 *
 *      start_offset - Supplies the start byte offset.
 *
 *      end_offset - Supplies the end byte offset.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */
{
    bool status;
    CURLcode res;
    char range_request[VHD_SYNC_XT_HTTP_HEADER_REQ_SIZE];

    status = false;

    snprintf(range_request,
             VHD_SYNC_XT_HTTP_HEADER_REQ_SIZE,
             "%lu-%lu",
             start_offset,
             end_offset);

    res = curl_easy_setopt(curl_config->curlhandle,
                           CURLOPT_RANGE,
                           range_request
                           );
    if (res != CURLE_OK)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_set_curl_data_range: Could not set range.\n");
        status = false;
        goto End;
    }

    status = true;

End:
    return status;
}
