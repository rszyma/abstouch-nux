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
#include "display.h"
#include "../config.h"
#include "../print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <X11/Xatom.h>

/*
 * Parses the `name` to Xatom in `display`.
 */
static Atom parse_xatom(Display *display, char *name)
{
    int is_atom = 1;
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isdigit(name[i])) {
            is_atom = 0;
            break;
        }
    }

    if (is_atom)
        return atoi(name);
    else
        return XInternAtom(display, name, 0);
}

/*
 * Returns true if the display is running on XWayland.
 */
int LIsXWayland(Display *display)
{
    XDeviceInfo *devices;
    int is_xwayland = 0;
    int n;

    devices = XListInputDevices(display, &n);
    while (n-- > 0) {
        if (!strncmp(devices[n].name, "xwayland-", 9)) {
            is_xwayland = 1;
            break;
        }
    }

    XFreeDeviceList(devices);
    return is_xwayland;
}

/*
 * Returns true if dirent starts with display prefix.
 */
int LIsDisplayDevice(const struct dirent *dir)
{
    return !strncmp(DISPLAY_DEV_PREFIX, dir->d_name, strlen(DISPLAY_DEV_PREFIX));
}

/*
 * Returns the XInput device with `name` on `display`.
 */
XDevice *LOpenXDevice(Display *display, char *name)
{
    XDeviceInfo *info;
    int num_devices;

    info = XListInputDevices(display, &num_devices);
    for (int i = 0; i < num_devices; i++) {
        if (info[i].use >= IsXExtensionDevice) {
            if (!strcmp(info[i].name, name))
                return XOpenDevice(display, info[i].id);
            else {
                XID id = info[i].id;
                char device_id[256];
                snprintf(device_id, sizeof(device_id), "%lu", id);
                if (!strcmp(name, device_id))
                    return XOpenDevice(display, id);
            }
        }
    }

    return NULL;
}

/*
 * Sets the enabled property of the XInput `device` on `display`.
 */
int LSetXDeviceEnabled(Display *display, XDevice *device, int enabled)
{
    char *value = calloc(1, sizeof(long));
    value[0] = enabled;
    XChangeDeviceProperty(display, device, parse_xatom(display, "Device Enabled"), XA_INTEGER, 8, PropModeReplace, value, 1);
    XSync(display, 0);
    return EXIT_SUCCESS;
}

/*
 * Sets the configuration about display interactively.
 */
int LSetDisplayInteractive(void)
{
    EConfig config = CGetConfig();
    struct dirent **namelist;
    int ndev = scandir(DISPLAY_DEV_DIR, &namelist, LIsDisplayDevice, versionsort);
    if (ndev < 1) {
        ERRLN("Couldn't get displays.");
        LOGLN("Try running as root.");
        return EXIT_FAILURE;
    }

    size_t displays_len = 0;
    char **displays = malloc(displays_len);
    size_t screens_len = 0;
    int *screens = malloc(screens_len);
    LOGLN("Displays:");
    for (int i = 0; i < ndev; i++) {
        char display_name[64] = ":";
        strcat(display_name, namelist[i]->d_name + 1);

        Display *display = XOpenDisplay(display_name);
        if (display == NULL)
            continue;
        
        displays = realloc(displays, ++displays_len * sizeof(char *));
        char *buf = malloc(strlen(display_name) + 1);
        strcpy(buf, display_name);
        displays[displays_len - 1] = buf;
        PRINTLN("   - Display \x1b[0;37m%s %s", display_name, LIsXWayland(display) ? "(XWayland)": "");
        
        int count = XScreenCount(display);
        screens = realloc(screens, ++screens_len * sizeof(int));
        screens[screens_len - 1] = count;
        for (int j = 0; j < count; j++)
            PRINTLN("     - Screen \x1b[0;37m%d => \x1b[0;37m%d\x1b[1;32mx\x1b[0;37m%d",
                j, XDisplayWidth(display, j), XDisplayHeight(display, j));
        XCloseDisplay(display);
    }

    char *prefix = ":";
    for (int i = 0; i < displays_len; i++) {
        if (strncmp(displays[i], ":", 1)) {
            strcpy(prefix, "");
            break;
        }
    }

    LOG("Please enter the display name. => \x1b[;m%s", prefix);
    char *p, display_name[256], s[256];
    int screen, display_idx;
    while (fgets(s, sizeof(s), stdin)) {
        s[strcspn(s, "\n")] = 0;
        snprintf(display_name, sizeof(display_name), "%s%s", prefix, s);
        
        int available = 0;
        for (int i = 0; i < displays_len; i++) {
            if (!strcmp(display_name, displays[i])) {
                display_idx = i;
                available = 1;
                break;
            }
        }

        if (available)
            break;
        
        CUP(1);
        LOGCLEAR("Please enter the display name. => \x1b[;m%s", prefix);
    }
    config.display = display_name;

    LOG("Please enter the screen id. => ");
    while (fgets(s, sizeof(s), stdin)) {
        screen = strtol(s, &p, 10);
        if (p == s || *p != '\n') {
            CUP(1);
            LOGCLEAR("Enter the screen id. => ");
            continue;
        }

        if (screens[display_idx] > screen)
            break;

        CUP(1);
        LOGCLEAR("Please enter the screen id. => ");
    }
    config.screen = screen;

    SUCCESSLN("Successfully set display configuration.");
    CSetConfig(config);
    free(displays);
    free(screens);
    return EXIT_SUCCESS;
}
