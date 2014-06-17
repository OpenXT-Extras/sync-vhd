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
 * This is the file that contains the functions to parse the commandline options
 * passed to the tool. 
 *
 *
 *   Sharath George (t_sharathg) Jan 2012
 */

/* ---------------- Header includes ---------------------------------------- */

#include <vhdsyncxt_args.h>

/* ---------------- Constant/Global Declarations --------------------------- */

static const char* 
vhd_sync_xt_usage =
	"Usage %s: [OPTION [ARG]]... URL\n" \
	"\n"\
	"  Sync a VHD file with a URL resource on HTTP. 	\n\n"\
	"  --download                  Specifies the download action.\n"\
	"  --uuid  [uuid.vhd]          Specifies the disk to sync.\n"\
	"  --path  [local path]        Specifies the local path to sync the file to.\n"\
    "                                  Defaults to .\n"\
	"  --url   [server url]        Specifies the server URL.\n"\
	"  --progressfd [filedes]      Specifies the file descriptor to send progress notifications.\n"\
	"  --connectionfd [filedes]    Specifies the connected socket to send/recieve data from the server\n"\
	"  --cacert [certificate file] Specifies the certificate file of the server.\n"\
	"  --capath [ca path]          Specifies the certificate path of the server cert.\n"\
    "  --credentials [<username>:<passwd>] Specifies the login credentials for the server.\n";


typedef enum
_vhd_sync_xt_option_enum
{
    OPTION_LONG_FLAG = 0,
    OPTION_DOWNLOAD = 100,    // arbitrary constant
    OPTION_UPLOAD,
    OPTION_UPGRADE,
    OPTION_BACKUP,
    OPTION_HELP,
    OPTION_UUID,
    OPTION_URL,
    OPTION_LOCAL_PATH,
    OPTION_PROGRESS_FD,
    OPTION_CONNECTION_SOCKET,
    OPTION_CA_CERT,
    OPTION_CA_PATH,
    OPTION_CREDENTIALS
}vhd_sync_xt_option_enum, *pvhd_sync_xt_option_enum;

struct option 
vhd_sync_xt_option_flags[] = 
{
    {"download",        no_argument,        0,  OPTION_DOWNLOAD},
    {"upload",          no_argument,        0,  OPTION_UPLOAD},
    {"upgrade",         no_argument,        0,  OPTION_UPGRADE},
    {"backup",          no_argument,        0,  OPTION_BACKUP},
    {"help",            no_argument,        0,  OPTION_HELP},
    {"url",             required_argument,  0,  OPTION_URL},
    {"uuid",            required_argument,  0,  OPTION_UUID},
    {"path",            required_argument,  0,  OPTION_LOCAL_PATH},
    {"progressfd",      required_argument,  0,  OPTION_PROGRESS_FD},
    {"connectionfd",    required_argument,  0,  OPTION_CONNECTION_SOCKET},
    {"cacert",          required_argument,  0,  OPTION_CA_CERT},
    {"capath",          required_argument,  0,  OPTION_CA_PATH},
    {"credentials",     required_argument,  0,  OPTION_CREDENTIALS},
	{0,}
};

/* ---------------- Function Definitions ----------------------------------- */

bool
vhd_sync_xt_create_parameters(
    pvhd_sync_xt_parameters* parameters
    )
/*
 * This function creates a parameters struct to store the parsed arguments.
 *
 * Parameters:
 *
 *      parameters - Supplies a placeholder to return the parameters struct
 *          that was created.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */  
{
    bool status;
    pvhd_sync_xt_parameters    parameters_local;

    status = false;
    parameters_local = NULL;

    parameters_local = calloc(1, sizeof(vhd_sync_xt_parameters));
    if (parameters_local == NULL)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_create_parameters: Could not allocate memory for parameters struct.\n");
        goto End;
    }

    //
    // Set the default parameters.
    //
    parameters_local->progress_fd =  0;
    parameters_local->connection_socket = 0;
    parameters_local->localpath = VHD_SYNC_XT_DEFAULT_PATH;

    *parameters = parameters_local;
    parameters_local = NULL;
    status = true;   

End:
    if (parameters_local != NULL)
    {
        free(parameters_local);
    }

    return status;
}


void
vhd_sync_xt_destroy_parameters(
    pvhd_sync_xt_parameters parameters
    )
/*
 * This function destroys a parameters struct that stores the parsed arguments.
 *
 * Parameters:
 *
 *      parameters - Supplies a poitner to the parameters struct.
 *
 * Return Value:
 *
 *      None.
 */  
{
    if (parameters != NULL)
    {
        free (parameters);
    }
}


void 
vhd_sync_xt_show_help(
    pvhd_sync_xt_parameters parameters
    )
/*
 * This function prints the usage help for the tool.
 *
 * Parameters:
 *
 *      parameters - Supplies a pointer to the parameter struct.
 *
 * Return Value:
 *
 *      None.
 */      
{
	char *basename_local;
    
    //
    // Pull the name of the invoked command.
    //
    basename_local = basename(parameters->argv[0]);

	fprintf(stderr, 
            vhd_sync_xt_usage, 
            basename_local?basename_local:VHD_SYNC_XT_DEFAULT_NAME);

}

bool
vhd_sync_xt_check_parameters(
    pvhd_sync_xt_parameters parameters
    )
/*
 * This function checks the sanity of the parsed parameters.
 *
 * Parameters:
 *
 *      parameters - Supplies a pointer to the parameter struct.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */ 

{
    bool status;
    status = false;

    if (parameters->action >= ACTION_MINIMUM 
        && parameters->action < ACTION_MAXIMUM)
    {
        status = true;
    }

    if (parameters->action == ACTION_SHOWHELP)
    {
        goto End;
    }
    else if (parameters->action == ACTION_DOWNLOAD)
    {
        if (parameters->url == NULL)
        {
            VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_check_parameters: Download action should have url.\n");
            status = false;
            goto End;
        }

        if (parameters->imageuuid == NULL)
        {
            VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_check_parameters: Download action should have uuid.\n");
            status = false;
            goto End;
        }

    }
    //
    // Add conditions for other actions here.
    //

    
End:
    return status;    
}

bool
vhd_sync_xt_parse_parameters(
    pvhd_sync_xt_parameters parameters,
    int argc, 
    char *argv[] 
    )
/*
 * This function parses the command line options and sets the state of the 
 * global configuration state that the tool uses.
 *
 * Parameters:
 *
 *      parameters - Supplies a pointer to the parameter struct.
 *
 *      argc - Supplies the argument count.
 *
 *      argv -  Supplies the argument strings.
 *
 * Return Value:
 *
 *      TRUE on success, FALSE otherwise.
 */ 
{
    bool status;
    int option_index;
    int c;

    status = false;

	if (parameters == NULL)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_parse_parameters: No parameters struct passed in.\n");
        status = false;
        goto End;
    }

    //
    // Cache the commandline options in the struct.
    //
    parameters->argc = argc;
    parameters->argv = argv;


    while (1)
    {
        c = getopt_long_only(argc, argv, "", vhd_sync_xt_option_flags, &option_index);

        //
        // Break out of the while loop if we have hit the last index.
        //
        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case '?':
                parameters->action = ACTION_SHOWHELP;
                break;

            case OPTION_HELP:
                parameters->action = ACTION_SHOWHELP;
                break;

            case OPTION_DOWNLOAD:
                parameters->action = ACTION_DOWNLOAD;
                break;
            
            case OPTION_UPLOAD:
                parameters->action = ACTION_UPLOAD;
                break;

            case OPTION_UPGRADE:
                parameters->action = ACTION_UPGRADE;
                break;

            case OPTION_BACKUP:
                parameters->action = ACTION_BACKUP;
                break;

            case OPTION_URL:
                parameters->url = optarg;
                break;

            case OPTION_UUID:
                parameters->imageuuid = optarg;
                break;

            case OPTION_LOCAL_PATH:
                parameters->localpath = optarg;
                break;

            case OPTION_PROGRESS_FD:
                parameters->progress_fd = atoi(optarg);
                break;

            case OPTION_CONNECTION_SOCKET:
                parameters->connection_socket = atoi(optarg);
                break;

            case OPTION_CA_CERT:
                parameters->ca_cert = optarg;
                break;

            case OPTION_CA_PATH:
                parameters->ca_path = optarg;
                break;

            case OPTION_CREDENTIALS:
                parameters->credentials = optarg;
                break;

            default:
                VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_parse_parameters: getopt returned code : %d .\n", c);
                status = false;
                goto End;

        }

    }

    //
    // Sanity check our parameters.
    //
    status = vhd_sync_xt_check_parameters(parameters);
    if (status == false)
    {
        VHD_SYNC_XT_ERRORLOG("vhd_sync_xt_parse_parameters: parameter check failed.\n");
        goto End;
    }

    status = true;

End:    
	return status;	
}
