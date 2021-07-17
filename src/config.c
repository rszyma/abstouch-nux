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

#include "config.h"
#include "getch.h"
#include "print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "linux/event.h"
#include "linux/display.h"
#include "linux/client.h"
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

/*
 * Interrupt signal handler that while loops depend on.
 */
static volatile sig_atomic_t stop = 0;

/*
 * Sets `stop` to true to stop loop on interrupt or termination.
 */
static void signal_handler(int _sig)
{
    stop = 1;
}

/*
 * Returns the configuration directory.
 */
char *CGetConfigDir(void)
{
    char *home;
    if ((home = getenv("HOME")) == NULL)
        home = getpwuid(getuid())->pw_dir;
    
    size_t configdir_len = strlen(home) + strlen("/.config/abstouch-nux") + 1;
    char *configdir = malloc(configdir_len);
    snprintf(configdir, configdir_len, "%s/.config/abstouch-nux", home);
    
    struct stat st = {0};
    if (stat(configdir, &st) == -1)
        mkdir(configdir, 0700);
    return configdir;
}

/*
 * Returns true if config file exists.
 */
int CConfigExists(char *config)
{
    char path[4096];
    snprintf(path, 4096, "%s/%s.conf", CGetConfigDir(), config);
    return access(path, F_OK) == 0;
}

/*
 * Returns the saved configuration in the config file.
 */
EConfig CGetConfig(void)
{
    EConfig config = {.event = 0, .event_name = "",
        .display = ":0", .screen = 0,
        .use_defaults = 0,
        .x_min = 0, .x_max = 0,
        .y_min = 0, .y_max = 0,
        .error = 0};
    if (!CConfigExists("abstouch-nux")) {
        config.error = 1;
        return config;
    }

    char path[4096];
    snprintf(path, 4096 , "%s/abstouch-nux.conf", CGetConfigDir());
    char key[256], val[256];
    char *p;

    FILE *f = fopen(path, "r");
    while (fscanf(f, "%255[^=]=%255[^\n]%*c", key, val) == 2) {
        if (!strcmp(key, "event"))
            config.event = (int) strtol(val, &p, 10);
        else if (!strcmp(key, "event_name"))
            strcpy((config.event_name = malloc(sizeof(val))), val);
        else if (!strcmp(key, "display")) 
            strcpy((config.display = malloc(sizeof(val))), val);
        else if (!strcmp(key, "screen"))
            config.screen = (int) strtol(val, &p, 10);
        else if (!strcmp(key, "use_defaults"))
            config.use_defaults = (int) strtol(val, &p, 10);
        else if (!strcmp(key, "x_min"))
            config.x_min = (int) strtol(val, &p, 10);
        else if (!strcmp(key, "x_max"))
            config.x_max = (int) strtol(val, &p, 10);
        else if (!strcmp(key, "y_min"))
            config.y_min = (int) strtol(val, &p, 10);
        else if (!strcmp(key, "y_max"))
            config.y_max = (int) strtol(val, &p, 10);
    }
    fclose(f);

    config.error = 0;
    return config;
}

/*
 * Saves the configuration to the config file.
 */
int CSetConfig(EConfig config)
{
    char path[4096];
    snprintf(path, 4096, "%s/abstouch-nux.conf", CGetConfigDir());

    FILE *f = fopen(path, "w");
    fprintf(f, "event=%d\n", config.event);
    fprintf(f, "event_name=%s\n", config.event_name);
    fprintf(f, "display=%s\n", config.display);
    fprintf(f, "screen=%d\n", config.screen);
    fprintf(f, "use_defaults=%d\n", config.use_defaults);
    fprintf(f, "x_min=%d\n", config.x_min);
    fprintf(f, "x_max=%d\n", config.x_max);
    fprintf(f, "y_min=%d\n", config.y_min);
    fprintf(f, "y_max=%d\n", config.y_max);
    fclose(f);
    return EXIT_SUCCESS;
}

/*
 * Sets the configuration about input events interactively.
 */
int CSetEventInteractive(void)
{
    return LSetEventInteractive();
}

/*
 * Sets the configuration about display interactively.
 */
int CSetDisplayInteractive(void)
{
    return LSetDisplayInteractive();
}

/*
 * Calibrate the touchpad and set the configuration about limits.
 */
int CCalibrate(int visual)
{
    return LCalibrate(visual);
}

/*
 * Changes or shows the configuration interactively.
 */
int CConfigInteractive()
{
    EConfig config = CGetConfig();
    if (config.error)
        CSetConfig(config);
    
    /* Total lines and the key count in the configuration menu to use in calculations. */
    int lines = 13;
    int key_count = 9;

    /* 2D array that holds information about the config keys. */
    EConfigKey keys[] = {
        {.pointer_int = &config.event, .type = 0},
        {.pointer_str = &config.event_name, .type = 1},
        {.pointer_str = &config.display, .type = 1},
        {.pointer_int = &config.screen, .type = 0},
        {.pointer_int = &config.use_defaults, .type = 2},
        {.pointer_int = &config.x_min, .type = 0},
        {.pointer_int = &config.x_max, .type = 0},
        {.pointer_int = &config.y_min, .type = 0},
        {.pointer_int = &config.y_max, .type = 0}
    };

    PRINTLN("---===abstouch-nux=Configuration===---");
    for (int i = 0; i < key_count; i++)
        printf("\n");
    printf("\n");
    PRINTLN("---================================---\n");
    LOGLN("Use WASD to move, Enter to edit and ESC to exit.");

    int idx = 0;
    int edit = 0;
    signal(SIGINT, signal_handler);
    while (!stop) {
        CUP(lines);
        LOGLNCLEAR("Event = \x1b[0;37m%d", config.event);
        LOGLNCLEAR("Event Name = \"\x1b[0;37m%s\"", config.event_name);
        LOGLNCLEAR("Display = \"\x1b[0;37m%s\"", config.display);
        LOGLNCLEAR("Screen = \x1b[0;37m%d", config.screen);
        LOGLNCLEAR("Use Defaults = \x1b[0;37m%s", config.use_defaults ? "Yes" : "No");
        LOGLNCLEAR("Min X = \x1b[0;37m%d", config.x_min);
        LOGLNCLEAR("Max X = \x1b[0;37m%d", config.x_max);
        LOGLNCLEAR("Min Y = \x1b[0;37m%d", config.y_min);
        LOGLNCLEAR("Max Y = \x1b[0;37m%d", config.y_max);
        CDOWN(4);
        CUP(lines);
        if (idx > 0)
            CDOWN(idx);
        /* Cursor */
        CLEFT(255);
        printf("  \x1b[1;35m=>");
        CDOWN(lines - idx);
        LCLEAR();

        if (!edit) {
            char act = getch();
            switch (act) {
                case 'W':
                case 'w':
                    idx -= 1;
                    if (idx < 0)
                        idx = key_count - 1;

                    break;
                case 'S':
                case 's':
                    idx += 1;
                    if (idx > key_count - 1)
                        idx = 0;

                    break;
                case ENTER_CHAR:
                    edit = 1;
                    break;
                case ESC_CHAR:
                    stop = 1;
                    break;
                default:
                    continue;
                    break;
            }

            continue;
        }

        switch (keys[idx].type) {
            case 0:
                CUP(2);
                LOGCLEAR("Please enter the new value. => ");
                int vali;

                char *p, s[64];
                while (fgets(s, sizeof(s), stdin)) {
                    vali = strtol(s, &p, 10);
                    if ((p == s || *p != '\n')) {
                        CUP(1);
                        LOGCLEAR("Please enter the new value. => ");
                        continue;
                    }

                    break;
                }
                CUP(1);
                LCLEAR();
                CDOWN(2);

                *(keys[idx].pointer_int) = vali;
                edit = 0;
                
                break;
            case 1:
                CUP(2);
                LOGCLEAR("Please enter the new value. => ");
                char vals[256];
                
                if (fgets(vals, sizeof(vals), stdin)) {
                    vals[strcspn(vals, "\n")] = 0;
                    break;
                }
                
                CUP(1);
                LCLEAR();
                CDOWN(2);

                strcpy(*(keys[idx].pointer_str), vals);
                edit = 0;

                break;
            case 2:
                *(keys[idx].pointer_int) = !*(keys[idx].pointer_int);
                edit = 0;
                break;
        }
    }

    CSetConfig(config);
    return EXIT_SUCCESS;
}
