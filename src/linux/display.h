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
#ifndef _LINUX_DISPLAY_H
#define _LINUX_DISPLAY_H

#include <dirent.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>

#define DISPLAY_DEV_DIR "/tmp/.X11-unix"
#define DISPLAY_DEV_PREFIX "X"
#define DISPLAY_NAME_PREFIX ":"

/*
 * Returns true if the display is running on XWayland.
 */
int LIsXWayland(Display *display);

/*
 * Returns true if dirent starts with display prefix.
 */
int LIsDisplayDevice(const struct dirent *dir); 

/*
 * Returns the XInput device with `name` on `display`.
 */
XDevice *LOpenXDevice(Display *display, char *name);

/*
 * Sets the enabled property of the XInput `device` on `display`.
 */
int LSetXDeviceEnabled(Display *display, XDevice *device, int enabled);

/*
 * Sets the configuration about display interactively.
 */
int LSetDisplayInteractive(void);

#endif /* _LINUX_DISPLAY_H */
