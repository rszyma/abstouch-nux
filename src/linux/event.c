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
#define _GNU_SOURCE
#include "event.h"
#include "../config.h"
#include "../print.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)
#define NAME_ELEMENT(element) [element] = #element

/*
 * A basic list of used events.
 */
const char *const events[EV_MAX + 1] = {
    [0 ... EV_MAX] = NULL,
    NAME_ELEMENT(EV_KEY),
    NAME_ELEMENT(EV_ABS)
};

/*
 * A basic list of maximum values of used events.
 */
const int maxval[EV_MAX + 1] = {
    [0 ... EV_MAX] = -1,
    [EV_KEY] = KEY_MAX,
    [EV_ABS] = ABS_MAX
};

/*
 * A basic list of input codes in absolute input type.
 */
const char *const absolutes[ABS_MAX + 1] = {
    [0 ... ABS_MAX] = NULL,
    NAME_ELEMENT(ABS_X),            NAME_ELEMENT(ABS_Y),
    NAME_ELEMENT(ABS_Z),            NAME_ELEMENT(ABS_RX),
    NAME_ELEMENT(ABS_RY),           NAME_ELEMENT(ABS_RZ),
    NAME_ELEMENT(ABS_THROTTLE),     NAME_ELEMENT(ABS_RUDDER),
    NAME_ELEMENT(ABS_WHEEL),        NAME_ELEMENT(ABS_GAS),
    NAME_ELEMENT(ABS_BRAKE),        NAME_ELEMENT(ABS_HAT0X),
    NAME_ELEMENT(ABS_HAT0Y),        NAME_ELEMENT(ABS_HAT1X),
    NAME_ELEMENT(ABS_HAT1Y),        NAME_ELEMENT(ABS_HAT2X),
    NAME_ELEMENT(ABS_HAT2Y),        NAME_ELEMENT(ABS_HAT3X),
    NAME_ELEMENT(ABS_HAT3Y),        NAME_ELEMENT(ABS_PRESSURE),
    NAME_ELEMENT(ABS_DISTANCE),     NAME_ELEMENT(ABS_TILT_X),
    NAME_ELEMENT(ABS_TILT_Y),       NAME_ELEMENT(ABS_TOOL_WIDTH),
    NAME_ELEMENT(ABS_VOLUME),       NAME_ELEMENT(ABS_MISC),
#ifdef ABS_MT_BLOB_ID
    NAME_ELEMENT(ABS_MT_TOUCH_MAJOR),
    NAME_ELEMENT(ABS_MT_TOUCH_MINOR),
    NAME_ELEMENT(ABS_MT_WIDTH_MAJOR),
    NAME_ELEMENT(ABS_MT_WIDTH_MINOR),
    NAME_ELEMENT(ABS_MT_ORIENTATION),
    NAME_ELEMENT(ABS_MT_POSITION_X),
    NAME_ELEMENT(ABS_MT_POSITION_Y),
    NAME_ELEMENT(ABS_MT_TOOL_TYPE),
    NAME_ELEMENT(ABS_MT_BLOB_ID),
#endif
#ifdef ABS_MT_TRACKING_ID
    NAME_ELEMENT(ABS_MT_TRACKING_ID),
#endif
#ifdef ABS_MT_PRESSURE
    NAME_ELEMENT(ABS_MT_PRESSURE),
#endif
#ifdef ABS_MT_SLOT
    NAME_ELEMENT(ABS_MT_SLOT),
#endif
#ifdef ABS_MT_TOOL_X
    NAME_ELEMENT(ABS_MT_TOOL_X),
    NAME_ELEMENT(ABS_MT_TOOL_Y),
    NAME_ELEMENT(ABS_MT_DISTANCE)
#endif
};

/*
 * A basic list of used input keys.
 */
const char *const keys[KEY_MAX + 1] = {
    [0 ... KEY_MAX] = NULL,
    NAME_ELEMENT(BTN_0),                NAME_ELEMENT(BTN_1),
    NAME_ELEMENT(BTN_2),                NAME_ELEMENT(BTN_3),
    NAME_ELEMENT(BTN_4),                NAME_ELEMENT(BTN_5),
    NAME_ELEMENT(BTN_6),                NAME_ELEMENT(BTN_7),
    NAME_ELEMENT(BTN_8),                NAME_ELEMENT(BTN_9),
    NAME_ELEMENT(BTN_LEFT),             NAME_ELEMENT(BTN_RIGHT),
    NAME_ELEMENT(BTN_MIDDLE),           NAME_ELEMENT(BTN_SIDE),
    NAME_ELEMENT(BTN_EXTRA),            NAME_ELEMENT(BTN_FORWARD),
    NAME_ELEMENT(BTN_BACK),             NAME_ELEMENT(BTN_TASK),
    NAME_ELEMENT(BTN_TRIGGER),          NAME_ELEMENT(BTN_THUMB),
    NAME_ELEMENT(BTN_THUMB2),           NAME_ELEMENT(BTN_TOP),
    NAME_ELEMENT(BTN_TOP2),             NAME_ELEMENT(BTN_PINKIE),
    NAME_ELEMENT(BTN_BASE),             NAME_ELEMENT(BTN_BASE2),
    NAME_ELEMENT(BTN_BASE3),            NAME_ELEMENT(BTN_BASE4),
    NAME_ELEMENT(BTN_BASE5),            NAME_ELEMENT(BTN_BASE6),
    NAME_ELEMENT(BTN_DEAD),             NAME_ELEMENT(BTN_C),

    NAME_ELEMENT(BTN_Z),                NAME_ELEMENT(BTN_TL),
    NAME_ELEMENT(BTN_TR),               NAME_ELEMENT(BTN_TL2),
    NAME_ELEMENT(BTN_TR2),              NAME_ELEMENT(BTN_SELECT),
    NAME_ELEMENT(BTN_START),            NAME_ELEMENT(BTN_MODE),
    NAME_ELEMENT(BTN_THUMBL),           NAME_ELEMENT(BTN_THUMBR),
    NAME_ELEMENT(BTN_TOOL_PEN),         NAME_ELEMENT(BTN_TOOL_RUBBER),
    NAME_ELEMENT(BTN_TOOL_BRUSH),       NAME_ELEMENT(BTN_TOOL_PENCIL),
    NAME_ELEMENT(BTN_TOOL_AIRBRUSH),    NAME_ELEMENT(BTN_TOOL_FINGER),
    NAME_ELEMENT(BTN_TOOL_MOUSE),       NAME_ELEMENT(BTN_TOOL_LENS),
    NAME_ELEMENT(BTN_TOUCH),            NAME_ELEMENT(BTN_STYLUS),
    NAME_ELEMENT(BTN_STYLUS2),          NAME_ELEMENT(BTN_TOOL_DOUBLETAP),
    NAME_ELEMENT(BTN_TOOL_TRIPLETAP)
};

/*
 * A basic list of all used input name codes.
 */
const char *const *const names[EV_MAX + 1] = {
    [0 ... EV_MAX] = NULL,
    [EV_KEY] = keys,
    [EV_ABS] = absolutes
};

/*
 * Returns true if dirent starts with event prefix.
 */
int LIsEventDevice(const struct dirent *dir)
{
    return !strncmp(EVENT_PREFIX, dir->d_name, 5);
}

/*
 * Returns the name of the type from `type` id.
 */
const char *typename(unsigned int type)
{
    return (type <= EV_MAX && events[type]) ? events[type] : "?";
}

/*
 * Returns the name of the input `code` that is in `type`.
 */
const char *codename(unsigned int type, unsigned int code)
{
    return (type <= EV_MAX && code <= maxval[type] && names[type] && names[type][code]) ? names[type][code] : "?";
}

/*
 * Returns new fd of the event with given `event` id.
 */
int LOpenEvent(int event)
{
    int fd = -1;
    char fname[64];
    snprintf(fname, sizeof(fname),
        "%s/%s%d", DEV_INPUT_DIR, EVENT_PREFIX, event);
    fd = open(fname, O_RDONLY);
    return fd;
}

/*
 * Returns true if the input `event` has absolute input.
 */
int LIsAbsoluteEvent(int event)
{
    unsigned int type;
    unsigned long bit[EV_MAX][NBITS(KEY_MAX)];

    int fd = LOpenEvent(event);
    if (fd < 0)
        return 0;
    memset(bit, 0, sizeof(bit));
    ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
    close(fd);

    for (type = 0; type < EV_MAX; type++) {
        if (test_bit(type, bit[0]) && type != EV_REP) {
            if (type == EV_ABS)
                return 1;
        }
    }

    return 0;
}

/*
 * Scans all events and returns the event id with the given name `ename`.
 */
int LGetEventByName(char *ename)
{
    struct dirent **namelist;
    char *filename;
    int max_device = 0;

    int ndev = scandir(DEV_INPUT_DIR, &namelist, LIsEventDevice, versionsort);
    if (ndev < 1)
        return -1;

    for (int i = 0; i < ndev; i++) {
        char name[256];

        int fd = LOpenEvent(i);
        if (fd < 0) {
            if (i + 1 != ndev)
                ndev++;
            continue;
        }

        ioctl(fd, EVIOCGNAME(sizeof(name)), name);
        close(fd);

        if (!strcmp(ename, name))
            return i;
    }

    return -1;
}

/*
 * Returns the name of the event with `event` id.
 */
char *LGetEventName(int event)
{
    int fd = LOpenEvent(event);
    char *name = malloc(256);
    if (fd < 0)
        return name;
    
    ioctl(fd, EVIOCGNAME(256), name);
    close(fd);
    return name;
}

/*
 * Sets the configuration about input events interactively.
 */
int LSetEventInteractive(void)
{
    EConfig config = CGetConfig();
    struct dirent **namelist;
    int ndev, max = 0;

    ndev = scandir(DEV_INPUT_DIR, &namelist, LIsEventDevice, versionsort);
    if (ndev < 1) {
        ERRLN("Couldn't get events.");
        LOGLN("Try running as root.");
        return EXIT_FAILURE;
    }

    size_t ids_len = 0;
    int *ids = malloc(ids_len);
    LOGLN("Events:");
    for (int i = 0; i < ndev; i++) {
        char fname[256];
        char name[256] = "\x1b[1;31mUnknown\x1b[;m";
        
        int fd = -1;
        snprintf(fname, sizeof(fname), "%s/%s", DEV_INPUT_DIR, namelist[i]->d_name);
        fd = open(fname, O_RDONLY);
        if (fd < 0)
            continue;

        int id = 0;
        sscanf(namelist[i]->d_name, "event%d", &id);
        ids = realloc(ids, ++ids_len * sizeof(int));
        ids[ids_len - 1] = id;
        if (max < id)
            max = id;

        ioctl(fd, EVIOCGNAME(sizeof(name)), name);

        PRINTLN("   - \x1b[0;37m%d => \x1b[0;37m%s", id, name);
        close(fd);
    }

    int event;
    char *p, s[64];
    LOG("Please enter the event id. => [\x1b[0;37m0-\x1b[0;37m%d] => ", max);
    while (fgets(s, sizeof(s), stdin)) {
        event = strtol(s, &p, 10);
        if ((p == s || *p != '\n') || (event < 0 || event > max)) {
            CUP(1);
            LOGCLEAR("Please enter the event id. => [\x1b[0;37m0-\x1b[0;37m%d] => ", max);
            continue;
        }
        
        if (LIsAbsoluteEvent(event))
            break;

        CUP(1);
        ERRCLEAR("No absolute input found. Please enter the event id. => [\x1b[0;37m0-\x1b[0;37m%d] => ", max);
    }

    config.event = event;
    char name[256] = "?";
    int abs_x[6] = {0}, abs_y[6] = {0};
    int fd = LOpenEvent(event);
    ioctl(fd, EVIOCGNAME(sizeof(name)), name);
    ioctl(fd, EVIOCGABS(ABS_X), abs_x);
    ioctl(fd, EVIOCGABS(ABS_Y), abs_y);
    int x_min = abs_x[1], x_max = abs_x[2];
    int y_min = abs_y[1], y_max = abs_y[2];
    strcpy((config.event_name = malloc(256)), name);
    close(fd);
    SUCCESSLN("Successfully set input event.");

    config.x_min = x_min;
    config.x_max = x_max;
    config.y_min = y_min;
    config.y_max = y_max;

    SUCCESSLN("Successfully set the limits.");
    CSetConfig(config);
    free(ids);
    return EXIT_SUCCESS;
}
