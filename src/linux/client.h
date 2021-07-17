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
#ifndef _LINUX_CLIENT_H
#define _LINUX_CLIENT_H

#include "../config.h"

/*
 * Input client for GNU/Linux.
 */
int LInputClient(int verbose);

/*
 * Runs the input client for GNU/Linux as a daemon.
 */
int LInputClientDaemon(int verbose);

/*
 * Stop the input client for GNU/Linux on daemon. 
 */
int LStopInputClientDaemon(void);

/*
 * Calibrate the touchpad and set the configuration about limits on GNU/Linux.
 */
int LCalibrate(int visual);

#endif /* _LINUX_CLIENT_H */
