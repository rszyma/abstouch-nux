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

#include "getch.h"

#include <stdio.h>
#include <termios.h>

/*
 * Linux specific function to set termios settings and then getchar.
 */
static char getch_()
{
    struct termios old, cur;
    tcgetattr(0, &old);

    cur = old;
    cur.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &cur);

    char c = getchar();
    tcsetattr(0, TCSANOW, &old);
    return c;
}

/*
 * Get one char input without needing to press enter.
 */
char getch()
{
    char c = '?';
    c = getch_();
    return c;
}
