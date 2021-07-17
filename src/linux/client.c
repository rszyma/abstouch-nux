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
#include "client.h"
#include "event.h"
#include "display.h"
#include "../print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <math.h>

#include <linux/input.h>
#include <X11/extensions/XInput.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

static int gdaemon = 0;
static int gverbose = 0;

#define VERBOSE(fmt, args...) if (!gdaemon && gverbose) printf(fmt, ##args);

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
 * Input client for GNU/Linux.
 */
int LInputClient(int verbose)
{
    gverbose = verbose;

    EConfig config = CGetConfig();
    if (config.error) {
        if (!CConfigExists("abstouch-nux")) {
            ERRLN("abstouch-nux has not been set up.");
            LOGLN("See: \x1b[;mabstouch setup");
        } else
            ERRLN("Couldn't get the abstouch-nux configuration.");
        return EXIT_FAILURE;
    }

    int fd = LOpenEvent(config.event);
    if (fd < 0)
        return EXIT_FAILURE;

    if (!LIsAbsoluteEvent(config.event)) {
        int newevent = LGetEventByName(config.event_name);
        if (newevent < 0 || !LIsAbsoluteEvent(newevent)) {
            ERRLN("Event has no absolute input.");
            return EXIT_FAILURE;
        }

        config.event = newevent;
        CSetConfig(config);
        fd = LOpenEvent(newevent);
        if (fd < 0)
            return EXIT_FAILURE;
    }
    LOGLNIF(!gdaemon && gverbose, "Found absolute input on event \x1b[0;37m%d\x1b[1;37m.", config.event);

    Display *display = XOpenDisplay(config.display);
    Window root_window = XRootWindow(display, config.screen);
    XWindowAttributes window_attributes;
    XGetWindowAttributes(display, root_window, &window_attributes);
    XSelectInput(display, root_window, KeyReleaseMask);
    SUCCESSLNIF(!gdaemon && gverbose, "Successfully bound to display \x1b[0;37m%s\x1b[1;36m.\x1b[0;37m%d\x1b[1;37m.", config.display, config.screen);

    WARNLNIF(LIsXWayland(display), "Running on XWayland. All features might not be available.");
    if (LIsXWayland(display)) {
        ERRLN("XWayland is currently not supported for input.");
        return EXIT_FAILURE;
    }

    struct input_event ev[64];
    int rd;
    fd_set rdfs;
    FD_ZERO(&rdfs);
    FD_SET(fd, &rdfs);

    int abs_x[6] = {0}, abs_y[6] = {0}, abs_pressure[6] = {0};
    ioctl(fd, EVIOCGABS(ABS_X), abs_x);
    ioctl(fd, EVIOCGABS(ABS_Y), abs_y);
    ioctl(fd, EVIOCGABS(ABS_PRESSURE), abs_pressure);
    int x_min = config.x_min, x_max = config.x_max;
    int y_min = config.y_min, y_max = config.y_max;
    int pressure_min = abs_pressure[1], pressure_max = abs_pressure[2];
    int x, y, pressure;

    XDevice *device = LOpenXDevice(display, config.event_name);
    if (!config.use_defaults)
        LSetXDeviceEnabled(display, device, 0);

    stop = 0;
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    LOGLNIF(!gdaemon && gverbose, "Waiting for input...\n");
    while (!stop) {
        select(fd + 1, &rdfs, NULL, NULL, NULL);
        if (stop)
            break;
        rd = read(fd, ev, sizeof(ev));

        if (rd < (int) sizeof(struct input_event))
            return EXIT_FAILURE;

        for (int i = 0; i < rd / sizeof(struct input_event); i++) {
            unsigned int type, code;
            type = ev[i].type;
            code = ev[i].code;

            if (type != EV_ABS)
                continue;

            if (code == ABS_X) {
                x = ev[i].value;
                continue;
            }

            if (code == ABS_Y) {
                y = ev[i].value;
                continue;
            }

            if (code == ABS_PRESSURE) {
                pressure = ev[i].value;
                continue;
            }
        }

        if (!gdaemon && gverbose) {
            CUP(2);
            LCLEAR();
            SUCCESSLN("Got input at \x1b[0;37m%d\x1b[1;32mx\x1b[0;37m%d \x1b[1;37mwith \x1b[0;37m%d \x1b[1;37mpressure.\n", x, y, pressure);
        }

        int cx = (x - x_min) / ((x_max - x_min) / window_attributes.width);
        int cy = (y - y_min) / ((y_max - y_min) / window_attributes.height);
        XWarpPointer(display, None, root_window, 0, 0, 0, 0, cx, cy);
        XFlush(display);
        if (!gdaemon && gverbose) {
            CUP(1);
            LCLEAR();
            SUCCESSLN("Moved cursor to \x1b[0;37m%d\x1b[1;32mx\x1b[0;37m%d\x1b[1;37m.", cx, cy);
        }
    }

    LSetXDeviceEnabled(display, device, 1);
    return EXIT_SUCCESS;
}

/*
 * Runs the input client for GNU/Linux as a daemon.
 */
int LInputClientDaemon(int verbose)
{
    gdaemon = 1;
    gverbose = verbose;

    char *dir = CGetConfigDir();
    char path[strlen(dir) + strlen("/daemon.conf") + 1];
    snprintf(path, sizeof(path), "%s/daemon.conf", dir);
    free(dir);

    int restart = 0;
    restart = !LStopInputClientDaemon();

    pid_t daemon_pid = 0;
    daemon_pid = fork();

    if (daemon_pid < 0) {
        ERRLN("Couldn't start the abstouch-nux daemon.");
        return EXIT_FAILURE;
    }

    if (daemon_pid > 0) {
        FILE *f = fopen(path, "w");
        fprintf(f, "pid=%d\n", daemon_pid);
        fclose(f);

        if (verbose) {
            SUCCESSLNIF(restart, "Restarted the abstouch-nux daemon.");
            SUCCESSLNIF(!restart, "Started the abstouch-nux daemon.");
        }
        return EXIT_SUCCESS;
    }

    /* The code is running on daemon after this line. */
    return LInputClient(verbose);
}

/*
 * Stop the input client for GNU/Linux on daemon.
 */
int LStopInputClientDaemon(void)
{
    char *dir = CGetConfigDir();
    char path[strlen(dir) + strlen("/daemon.conf") + 1];
    snprintf(path, sizeof(path), "%s/daemon.conf", dir);
    free(dir);

    if (CConfigExists("daemon")) {
        char key[256], val[256];
        char *p;
        pid_t pid = 0;

        FILE *f = fopen(path, "r");
        while (fscanf(f, "%255[^=]=%255[^\n]%*c", key, val) == 2) {
            if (!strcmp(key, "pid"))
                pid = (int) strtol(val, &p, 10);
        }
        fclose(f);

        char name[1024];
        snprintf(name, sizeof(name), "/proc/%d/cmdline", pid);
        f = fopen(name, "r");
        if (f != NULL) {
            size_t size = fread(name, sizeof(char), 1024, f);
            name[size - 1] = '\0';
            fclose(f);

            if (strcasestr(name, "abstouch") != NULL) {
                kill(pid, SIGTERM);
                return remove(path);
            }
        }
    }

    return EXIT_FAILURE;
}
/*
 * Calibrate the touchpad and set the configuration about limits on GNU/Linux.
 */
int LCalibrate(int visual)
{
    EConfig config = CGetConfig();
    if (config.error) {
        if (!CConfigExists("abstouch-nux")) {
            ERRLN("abstouch-nux has not been set up.");
            LOGLN("See: \x1b[;mabstouch setup");
        } else
            ERRLN("Couldn't get the abstouch-nux configuration.");
        return EXIT_FAILURE;
    }

    int fd = LOpenEvent(config.event);
    if (fd < 0)
        return EXIT_FAILURE;

    if (!LIsAbsoluteEvent(config.event)) {
        int newevent = LGetEventByName(config.event_name);
        if (newevent < 0 || !LIsAbsoluteEvent(newevent)) {
            ERRLN("Event has no absolute input.");
            return EXIT_FAILURE;
        }

        config.event = newevent;
        CSetConfig(config);
        fd = LOpenEvent(newevent);
        if (fd < 0)
            return EXIT_FAILURE;
    }
    LOGLN("Found absolute input on event \x1b[;m%d\x1b[1;37m.", config.event);

    Display *display = XOpenDisplay(config.display);
    WARNLNIF(LIsXWayland(display), "Running on XWayland. All features might not be available.");
    if (LIsXWayland(display)) {
        ERRLN("XWayland is currently not supported for input.");
        return EXIT_FAILURE;
    }

    XDevice *device = LOpenXDevice(display, config.event_name);
    LSetXDeviceEnabled(display, device, 0);

    int new_x_min = 65536, new_x_max = -65535;
    int new_y_min = 65536, new_y_max = -65535;
    int new_x_size = 0, new_y_size = 0;

    struct input_event ev[64];
    int rd;
    fd_set rdfs;
    FD_ZERO(&rdfs);
    FD_SET(fd, &rdfs);
    int x, y;
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    if (visual) {
        LOGLN("Rub the touchpad until the visualization works correctly.");
        PRINT("\n\n\n\n\n"
              "          +--+\n"
              "          |  |\n"
              "          +--+\n"
              "\n\n\n\n\n");
    }

    int edited = 0;
    LOGLN("Waiting for input...");
    while (!stop) {
        select(fd + 1, &rdfs, NULL, NULL, NULL);
        if (stop)
            break;
        rd = read(fd, ev, sizeof(ev));

        if (rd < (int) sizeof(struct input_event))
            return EXIT_FAILURE;
        
        for (int i = 0; i < rd / sizeof(struct input_event); i++) {
            unsigned int type, code;
            type = ev[i].type;
            code = ev[i].code;

            if (type != EV_ABS)
                continue;

            if (code == ABS_X) {
                x = ev[i].value;
                continue;
            } else if (code == ABS_Y) {
                y = ev[i].value;
                continue;
            }
        }

        if (x < new_x_min) new_x_min = x;
        if (x > new_x_max) new_x_max = x;
        if (y < new_y_min) new_y_min = y;
        if (y > new_y_max) new_y_max = y;

        new_x_size = new_x_max - new_x_min;
        new_y_size = new_y_max - new_y_min;

        if (new_x_min == 65536 || new_x_max == -65535 || new_y_min == 65536 || new_y_max == -65535)
            continue;

        edited = 1;
        if (!visual) {
            CUP(1);
            LCLEAR();
            LOGLN("Press Ctrl + C to end the calibration. \t \x1b[0;37m%d\x1b[1;32mx\x1b[0;37m%d - \x1b[0;37m%d\x1b[1;32mx\x1b[0;37m%d", new_x_min, new_y_min, new_x_max, new_y_max);    
            continue;
        }

        double ratio = (double) new_x_size / (double) new_y_size;
        if (ratio > 3)
            ratio = 3;
        else if (ratio < 0)
            ratio = 0;

        int height = 10;
        int width = round(ratio) * height;
        if (width <= 0)
            continue;

        if (x == new_x_min || y == new_y_min)
            continue;

        if ((new_x_max - new_x_min) == 0)
            new_x_max += 1;
        if ((new_y_max - new_y_min) == 0)
            new_y_max += 1;

        int pos_x = round((x - new_x_min) / ((new_x_max - new_x_min) / width));
        int pos_y = height - round((y - new_y_min) / ((new_y_max - new_y_min) / height));
        CUP(1);
        for (int i = 0; i < height + 2; i++) {
            CUP(1);
            PRINTCLEAR(" ");
            if (i == 0 || i == (height + 1)) {
                PRINT("+");
                for (int j = 0; j < width; j++)
                    PRINT("--");
                PRINT("+");
                continue;
            }

            PRINT("|");
            for (int j = 0; j < width; j++) {
                if (i == pos_y && j == pos_x) {
                    PRINT("**");
                    continue;
                }
                printf("  ");
            }
            PRINT("|");
        }
        for (int i = 0; i < height + 2; i++)
            printf("\n");
        printf("\x1b[;m\n");

        CUP(1);
        LOGLNCLEAR("Press Ctrl + C to end the calibration. \t \x1b[0;37m%d\x1b[1;32mx\x1b[0;37m%d - \x1b[0;37m%d\x1b[1;32mx\x1b[0;37m%d", new_x_min, new_y_min, new_x_max, new_y_max);
    }

    if (!edited) {
        printf("\x1b[255D\x1b[K \x1b[1;32m=> \x1b[1;37mCancelled calibration.\x1b[;m\n");
        LSetXDeviceEnabled(display, device, 1);
        XCloseDevice(display, device);
        XCloseDisplay(display);
        return EXIT_SUCCESS;
    }

    config.x_min = new_x_min;
    config.x_max = new_x_max;
    config.y_min = new_y_min;
    config.y_max = new_y_max;
    CSetConfig(config);

    SUCCESSLNCLEAR("Successfully calibrated.");
    LSetXDeviceEnabled(display, device, 1);
    XCloseDevice(display, device);
    XCloseDisplay(display);
    return EXIT_SUCCESS;
}
