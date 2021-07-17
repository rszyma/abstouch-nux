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
#ifndef _PRINT_H
#define _PRINT_H

#include <stdio.h>

#define PRINT(fmt, args...) { printf(_color_string(fmt), ##args); }
#define LOG(fmt, args...) { printf(" \x1b[1;36m=> \x1b[1;37m"); PRINT(fmt, ##args); }
#define ERR(fmt, args...) { printf(" \x1b[1;31m=> \x1b[1;37m"); PRINT(fmt, ##args); }
#define SUCCESS(fmt, args...) { printf(" \x1b[1;32m=> \x1b[1;37m"); PRINT(fmt, ##args); }
#define WARN(fmt, args...) { printf(" \x1b[1,33m=> \x1b[1;37m"); PRINT(fmt, ##args); }

#define PRINTLN(fmt, args...) { PRINT(fmt, ##args); printf("\n"); }
#define LOGLN(fmt, args...) { LOG(fmt, ##args); printf("\n"); }
#define ERRLN(fmt, args...) { ERR(fmt, ##args); printf("\n"); }
#define SUCCESSLN(fmt, args...) { SUCCESS(fmt, ##args); printf("\n"); }
#define WARNLN(fmt, args...) { WARN(fmt, ##args); printf("\n"); }

#define PRINTIF(condition, fmt, args...) { if (condition) PRINT(fmt, ##args); }
#define LOGIF(condition, fmt, args...) { if (condition) LOG(fmt, ##args); }
#define ERRIF(condition, fmt, args...) { if (condition) ERR(fmt, ##args); }
#define SUCCESSIF(condition, fmt, args...) { if (condition) SUCCESS(fmt, ##args); }
#define WARNIF(condition, fmt, args...) { if (condition) WARN(fmt, ##args); }

#define PRINTLNIF(condition, fmt, args...) { if (condition) PRINTLN(fmt, ##args); }
#define LOGLNIF(condition, fmt, args...) { if (condition) LOGLN(fmt, ##args); }
#define ERRLNIF(condition, fmt, args...) { if (condition) ERRLN(fmt, ##args); }
#define SUCCESSLNIF(condition, fmt, args...) { if (condition) SUCCESSLN(fmt, ##args); }
#define WARNLNIF(condition, fmt, args...) { if (condition) WARNLN(fmt, ##args); }

#define LCLEAR() printf("\x1b[255D\x1b[K");
#define PRINTCLEAR(fmt, args...) { LCLEAR(); PRINT(fmt, ##args); }
#define LOGCLEAR(fmt, args...) { LCLEAR(); LOG(fmt, ##args); }
#define ERRCLEAR(fmt, args...) { LCLEAR(); ERR(fmt, ##args); }
#define SUCCESSCLEAR(fmt, args...) { LCLEAR(); SUCCESS(fmt, ##args); }

#define PRINTLNCLEAR(fmt, args...) { LCLEAR(); PRINTLN(fmt, ##args); }
#define LOGLNCLEAR(fmt, args...) { LCLEAR(); LOGLN(fmt, ##args); }
#define ERRLNCLEAR(fmt, args...) { LCLEAR(); ERRLN(fmt, ##args); }
#define SUCCESSLNCLEAR(fmt, args...) { LCLEAR(); SUCCESSLN(fmt, ##args); }

#define CUP(lines) if (lines > 0) printf("\x1b[%dA", lines);
#define CDOWN(lines) if (lines > 0) printf("\x1b[%dB", lines);
#define CRIGHT(lines) if (lines > 0) printf("\x1b[%dC", lines);
#define CLEFT(lines) if (lines > 0) printf("\x1b[%dD", lines);

/*
 * Struct that holds information about color formats.
 */
typedef struct {
    char **formats;
    size_t formats_len;
    char *color;
} _ColorFormat;

/*
 * Color the string using the following chart:
 * - []"<>/= => Bold Green (\x1b[1;32m)
 * - ',()-| => Green (\x1b[0;32m)
 * - "=>"+ => Bold Cyan (\x1b[1;36m)
 * - * => Bold Red (\x1b[1;31m)
 */
char *_color_string(char *string);

#endif /* _PRINT_H */
