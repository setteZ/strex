/*
* ----------------------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* <furgorsin@gmail.com> wrote this file. As long as you retain this
* notice you can do whatever you want with this stuff. If we meet some day,
* and you think this stuff is worth it, you can buy me a beer in return
* piero c
* ----------------------------------------------------------------------------
*
* Jul 2023
*/

/* ==========================       includes           ========================== */
#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "./lib/libGIS/ihex.h"


/* ==========================       typedefs           ========================== */

/* ==========================        defines           ========================== */

#define INPUT_FILE_DEFAULT       "input.txt"
#define OUTPUT_FILE_DEFAULT      "output.hex"
#define ADDRESS_OFFSET_DEFAULT   0
#define RECORD_TYPE              0
#define LINE_MAX_LEN             0x10

/* ==========================        typedefs          ========================== */

/* ==========================         enums            ========================== */

/* ==========================    global variables      ========================== */

/* ==========================    local variables       ========================== */
const char* rev = "0.1";

/* ==========================    function prototypes   ========================== */
void print_help(char **argv);
int address_arg_parser(char *arg);

/************************************** MAIN **************************************/
int main (int argc, char **argv)
{
    char * input_file = INPUT_FILE_DEFAULT;
    char * output_file = OUTPUT_FILE_DEFAULT;
    unsigned int address_offset = ADDRESS_OFFSET_DEFAULT;

    /** parse argument */
    while (1)
    {
        int c;
        static struct option long_options[] =
        {
            /* Funciotnality options*/
            {"address",  required_argument, 0, 'a'},
            {"input",    required_argument, 0, 'i'},
            {"output",   required_argument, 0, 'o'},

            /* Info options*/
            {"help",     no_argument,       0, 'h'},
            {"licence",  no_argument,       0, 'l'},
            {"version",  no_argument,       0, 'v'},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long_only (argc, argv, "a:i:o:hlv",
        long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
        break;

        switch (c) {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
            break;

            case 'a':
                address_offset = address_arg_parser(optarg);
            break;

            case 'h':
                print_help(argv);
                return EXIT_SUCCESS;
            break;

            case 'i':
                input_file = malloc((strlen(optarg) + 1) * sizeof(char));
                strcpy(input_file, optarg);
            break;

            case 'l':
                printf ("licence:\n");
                printf ("\"THE BEER-WARE LICENSE\" (Revision 42):\n");
                printf ("<furgorsin@gmail.com> wrote this file. As long as you retain this\n");
                printf ("notice you can do whatever you want with this stuff. If we meet some day,\n");
                printf ("and you think this stuff is worth it, you can buy me a beer in return\n");
                printf ("setteZ\n\n");
                return EXIT_SUCCESS;
            break;

            case 'o':
                output_file = malloc((strlen(optarg) + 1) * sizeof(char));
                strcpy(output_file, optarg);
            break;

            case 'v':
                printf ("%s - rev %s\n", argv[0], rev);
                return EXIT_SUCCESS;
            break;

            case '?':
            /* getopt_long already printed an error message. */
            break;

            default:
            abort ();
        }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        printf ("there's something more than I need: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        printf ("\n");
        return EXIT_FAILURE;
    }

    // disclaimer - start
    printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("!!     this software is for internal use only     !!\n");
    printf("!!       there is no warranty for its usage       !!\n");
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    // disclaimer - end

    {
        int i;
        char line[LINE_MAX_LEN + 2];
        FILE * in;
        FILE * out;

        in = fopen(input_file, "r");
        if(NULL == in)
        {
            printf("missing %s file\n", input_file);
            return EXIT_FAILURE;
        }

        out = fopen(output_file, "w");
        fclose(out);

        while(fgets(line, LINE_MAX_LEN, in) != NULL)
        {
            char * ret;
            int line_len;

            ret = strchr ((const char *)&line, '\n');
            if(NULL != ret)
            {
                *ret = '\0';
            }

            ret = strchr ((const char *)&line, '\r');
            if(NULL != ret)
            {
                *ret = '\0';
            }
            
            line_len = strlen(line); 
            if(line_len)
            {
                IHexRecord ihexRecord;

                (void)New_IHexRecord(0, address_offset, line, line_len + 1, &ihexRecord);
                out = fopen(output_file, "a");
                (void)Write_IHexRecord(&ihexRecord, out);
                fclose(out);
                address_offset += line_len + 1;
            }
        }

        out = fopen(output_file, "a");
        fprintf(out, ":00000001FF\r\n");
        fclose(out);

        fclose(in);
    }

    return EXIT_SUCCESS;
}

/**********************************************************************************/
/**********************************************************************************/

int address_arg_parser(char *arg)
{
    int base = 10;
    int arg_len = strlen(arg);

    if(arg_len > 1)
    {
        if(arg[arg_len - 1] == 'h')
        {
            /* 10h format */
            base = 16;
        }
        else if((arg[1] == 'x') || (arg[1] == 'X'))
        {
            /* 0x10 format */
            base = 16;
        }
    }

    return (int)strtol(arg, NULL, base);
}

void print_help(char **argv)
{
    printf("\n");
    printf("             HH  HH  EEEEE  LL     PPPP \n");
    printf("             HH  HH  EE     LL     PP  P\n");
    printf("             HHHHHH  EEEE   LL     PPPP \n");
    printf("             HH  HH  EE     LL     PP   \n");
    printf("             HH  HH  EEEEE  LLLLL  PP   \n\n");
    printf("%s convert a .txt file into a .hex file\n", argv[0]);
    printf("each line is saved as single string\n");
    printf("usage: %s <command> [<args>]\n\n", argv[0]);
    printf("Available commands:\n\n");
    printf("main functionality\n");
    printf("   -i <filename> the input txt file (default %d)\n", INPUT_FILE_DEFAULT);
    printf("   -o <filename> the input txt file (default %d)\n", OUTPUT_FILE_DEFAULT);
    printf("   -a <num>      the beginning memory address offset of the data\n");
    printf("                 hex format 0x10 and 10h accepted (default %d)\n", ADDRESS_OFFSET_DEFAULT);
    printf("\ninfo\n");
    printf("   -h            help screen\n");
    printf("   -l            license\n");
    printf("   -v            program version\n");
}
