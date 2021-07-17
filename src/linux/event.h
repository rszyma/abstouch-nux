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
#ifndef _LINUX_EVENT_H
#define _LINUX_EVENT_H

#include <dirent.h>
#include <linux/input.h>

#define DEV_INPUT_DIR "/dev/input"
#define EVENT_PREFIX "event"

/*
 * Returns true if dirent starts with event prefix.
 */
int LIsEventDevice(const struct dirent *dir); 

/*
 * Returns new fd of the event with given `event` id.
 */
int LOpenEvent(int event);

/*
 * Returns true if the input `event` has absolute input.
 */
int LIsAbsoluteEvent(int event);

/*
 * Scans all events and returns the event id with the given name `ename`.
 */
int LGetEventByName(char *ename);

/*
 * Returns the name of the event with `event` id.
 */
char *LGetEventName(int event);

/*
 * Sets the configuration about input events interactively.
 */
int LSetEventInteractive(void);

#endif /* _LINUX_EVENT_H */
