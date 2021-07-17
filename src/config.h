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
#ifndef _CONFIG_H
#define _CONFIG_H

/*
 * Struct that holds information about a config key.
 * Types:
 * - 0 = int
 * - 1 = char *
 * - 2 = bool (int)
 */
typedef struct {
    int *pointer_int;
    char **pointer_str;
    int type;
} EConfigKey;

/*
 * Basic struct that holds abstouch-nux configuration.
 */
typedef struct {
    int event;
    char *event_name;

    char *display;
    int screen;

    int use_defaults;

    int x_min;
    int x_max;
    int y_min;
    int y_max;

    int error;
} EConfig;

/*
 * Returns the configuration directory.
 */
char *CGetConfigDir(void);

/*
 * Returns true if the config file exists.
 */
int CConfigExists(char *config);

/*
 * Returns the saved configuration in the config file.
 */
EConfig CGetConfig(void);

/*
 * Saves the configuration to the config file.
 */
int CSetConfig(EConfig config);

/*
 * Sets the configuration about input events interactively.
 */
int CSetEventInteractive(void);

/*
 * Sets the configuration about display interactively.
 */
int CSetDisplayInteractive(void);

/*
 * Calibrate the touchpad and set the configuration about limits.
 */
int CCalibrate(int visual);

/*
 * Changes or shows the configuration interactively.
 */
int CConfigInteractive();

#endif /* _CONFIG_H */
