/****************************************************************************
** abstouch-nux - An absolute touchpad input client for GNU/Linux.
** Copyright (C) 2021  acedron <acedrons@yahoo.co.jp>
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <https://www.gnu.org/licenses/>.
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "linux/event.h"
#include "linux/client.h"

#include "config.h"
#include "print.h"

/*
 * Boolean options.
 */
static int verbose = 1;
static int daemon = 1;
static int visual = 1;

/*
 * Commands with the given name.
 */
static int setup(void);
static int start(void);
static int stop(void);
static int calibrate(void);
static int config(void);

int main(int argc, char **argv)
{
    size_t options_size = 0;
    char **options = malloc(options_size * sizeof(char *));
    size_t args_size = 0;
    char **args = malloc(args_size * sizeof(char *));
    for (int i = 1; i < argc; i++) {
        if (!strncmp(argv[i], "-", 1)) {
            if (!strncmp(argv[i], "--", 2)) {
                memmove(argv[i], argv[i] + 2, strlen(argv[i]));
                options = realloc(options, ++options_size * sizeof(char *));
                options[options_size - 1] = argv[i];
                continue;
            }

            memmove(argv[i], argv[i] + 1, strlen(argv[i]));
            for (int j = 0; j < strlen(argv[i]); j++) {
                char *opt = malloc(2);
                snprintf(opt, 2, "%c", argv[i][j]);
                options = realloc(options, ++options_size * sizeof(char *));
                options[options_size - 1] = opt;
            }
            continue;
        }

        args = realloc(args, ++args_size * sizeof(char *));
        args[args_size - 1] = argv[i];
    }

    if (args_size < 1) {
        ERRLN("No command provided.");
        LOGLN("See: \x1b[;mabstouch help");
        return EXIT_FAILURE;
    }

    char *command = args[0];
    for (int i = 0; i < args_size - 1; i++)
        args[i] = args[i + 1];
    args[args_size - 1] = NULL;
    args = realloc(args, --args_size * sizeof(char *));

    for (int i = 0; i < strlen(command); i++)
        command[i] = tolower(command[i]);

    if (!strcmp(command, "help")) {
        PRINTLN("---===abstouch-nux===---");
        LOGLN("An absolute touchpad input client for GNU/Linux.");
        printf("\n");
        PRINTLN("---=======Usage======---");
        LOGLN("abstouch <command> [\x1b[;moptions] [\x1b[;marguments]");
        printf("\n");
        PRINTLN("---=====Commands=====---");
        LOGLN("help => Shows this text.");
        LOGLN("start => Starts the abstouch-nux input client.");
        LOGLN("stop => Stops the abstouch-nux input client running as daemon.");
        LOGLN("setup => Runs the abstouch-nux setup.");
        LOGLN("calibrate => Calibrates the abstouch-nux input client.");
        LOGLN("config => Changes or shows the abstouch-nux configuration interactively.");
        printf("\n");
        PRINTLN("---===Options===---");
        LOGLN("-f,--foreground => Runs the client on foreground instead of background.");
        LOGLN("-q,--quiet => Disables the output with the client except errors.");
        LOGLN("--no-visual => Disables the visualization while calibrating.");
        printf("\n");
        PRINTLN("---=============---");
        return EXIT_SUCCESS;
    }

    for (int i = 0; i < options_size; i++) {
        if (!strcmp(options[i], "f") || !strcmp(options[i], "foreground"))
            daemon = 0;
        else if (!strcmp(options[i], "q") || !strcmp(options[i], "quiet"))
            verbose = 0;
        else if (!strcmp(options[i], "no-visual"))
            visual = 0;
    }

    if (!strcmp(command, "setup"))
        return setup();
    else if (!strcmp(command, "start"))
        return start();
    else if (!strcmp(command, "stop"))
        return stop();
    else if (!strcmp(command, "calibrate"))
        return calibrate();
    else if (!strcmp(command, "config"))
        return config();

    ERRLN("Unknown command: \x1b[;m%s", command);
    LOGLN("See: \x1b[;mabstouch help");
    return EXIT_FAILURE;
}

static int setup(void)
{
    CSetEventInteractive();
    CSetDisplayInteractive();

    EConfig config = CGetConfig();
    config.use_defaults = 1;
    CSetConfig(config);

    LOG("Would you like to calibrate now? => [Y/n] => ");
    char buf[256];
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        if (!strcmp(buf, "n\n") || !strcmp(buf, "N\n"))
            break;
        
        if (!strcmp(buf, "y\n") || !strcmp(buf, "Y\n") || !strcmp(buf, "\n")) {
            if (!strcmp(buf, "\n")) {
                CUP(1);
                LOGLNCLEAR("Would you like to calibrate now? => [Y/n] => y");
            }
       
            CCalibrate(visual);
            break;
        }

        CUP(1);
        LOGCLEAR("Would you like to calibrate now? => [Y/n] => ");
    }

    SUCCESSLN("Successfully set up abstouch-nux.");
    return EXIT_SUCCESS;
}

static int start(void)
{
    if (!daemon)
        return LInputClient(verbose);
    
    return LInputClientDaemon(verbose);
}

static int stop(void)
{
    int result = LStopInputClientDaemon();
    SUCCESSLNIF(verbose && !result, "Successfully stopped the abstouch-nux daemon.");
    ERRLNIF(verbose && result, "Couldn't find the abstouch-nux daemon.");
    return result;
}

static int calibrate(void)
{
    return CCalibrate(visual);
}

static int config(void)
{
    return CConfigInteractive();
}
