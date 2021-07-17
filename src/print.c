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

#include "print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Replaces the `rep` in `orig` with `with`.
 * Original code by jmucchiello in Stack Overflow.
 * https://stackoverflow.com/a/779960
 */
static char *str_replace(char *orig, char *rep, char *with)
{
    char *result, *ins, *tmp;
    int len_rep, len_with, len_front;
    int count;

    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL;
    if (!with)
        with = "";
    len_with = strlen(with);

    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count)
        ins = tmp + len_rep;

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep;
    }
    strcpy(tmp, orig);
    return result;
}

/*
 * Color the string using the following chart:
 * - []"<>/= => Bold Green (\x1b[1;32m)
 * - ',()-| => Green (\x1b[0;32m)
 * - "=>"+ => Bold Cyan (\x1b[1;36m)
 * - * => Bold Red (\x1b[1;31m)
 */
char *_color_string(char *string)
{
    char def[] = "\x1b[1;37m";
    _ColorFormat fmts[] = {
        {
            .formats = (char *[]) {"[", "]", "\"", "<", ">", "/", "="},
            .formats_len = 7,
            .color = "\x1b[1;32m"
        }, {
            .formats = (char *[]) {"\x1b[1;32m=\x1b[1;37m\x1b[1;32m>\x1b[1;37m", "+"},
            .formats_len = 2,
            .color = "\x1b[1;36m"
        }, {
            .formats = (char *[]) {"'", ",", "(", ")", "-", "|"},
            .formats_len = 6,
            .color = "\x1b[0;32m"
        }, {
            .formats = (char *[]) {"*"},
            .formats_len = 1,
            .color = "\x1b[1;31m"
        }
    };
    size_t fmts_len = 4;

    for (int i = 0; i < fmts_len; i++) {
        for (int j = 0; j < fmts[i].formats_len; j++) {
            char output[strlen(fmts[i].color) + strlen(fmts[i].formats[j]) + strlen(def) + 1];
            snprintf(output, sizeof(output), "%s%s%s", fmts[i].color, fmts[i].formats[j], def);

            string = str_replace(string, fmts[i].formats[j], output);
            if (!strcmp(fmts[i].formats[j], "\x1b[1;32m=\x1b[1;37m\x1b[1;32m>\x1b[1;37m"))
                string = str_replace(string, fmts[i].formats[j], "=>");

            if (!strcmp(fmts[i].formats[j], "[")) {
                /* [ is an exception because it is also used in escapes. */
                /* Revert the change back if [ is in an escape. */
                char rev[strlen("\x1b") + strlen(fmts[i].color) + strlen("[") + strlen(def) + 1];
                snprintf(rev, sizeof(rev), "\x1b%s[%s", fmts[i].color, def);
                string = str_replace(string, rev, "\x1b[");
            }
        }
    }

    return string;
}
