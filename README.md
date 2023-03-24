<h1 align="center">
    abstouch-nux
</h1>
<h3 align="center">
    An absolute touchpad input client for GNU/Linux.
</h3>

<h2 align="center"> Info </h2>
This project is forked from 

[acedron/abstouch-nux](https://github.com/acedron/abstouch-nux)

They stated that the project is discontinued (and is indeed archived), so I couldn't make PR directly.
I fixed a bug for calibration to work properly.

I also tested the program in game called osu!lazer. 
All you need to for abstouch to work with osu is to disable `high precision mouse` in game options

The program was tested with following touchpads:
- ELAN Touchpad

<h2 align="center"> Installation </h2>

For now, the only way you can install abstouch is to build the project from source. Here's how to do it:

[CMake](https://cmake.org) is recommended compiler.

You should install the dependencies first.
- **Arch Linux**: `$ sudo pacman -Sy cmake gcc libxi libx11 xf86-input-libinput --needed`
- **Debian/Ubuntu**: `$ sudo apt-get install cmake gcc libxi-dev libx11-dev libxi6 libx11-6 xserver-xorg-input-libinput`
- **Fedora/Red Hat**: `$ sudo dnf install cmake gcc libXi-devel libX11-devel libXi libX11 xorg-x11-drv-libinput`
- **openSUSE**: `$ sudo zypper install cmake gcc libXi-devel libX11-devel libXi6 libX11-6 xf86-input-libinput`

Then you can build the package.

```bash
$ git clone https://github.com/rszyma/abstouch.git
$ cd abstouch
$ cmake -B build
$ cmake --build build
$ sudo cmake --install build
```

**Make sure to add the user into the `input` group.**

```bash
$ sudo usermod -aG input $(whoami)
```
</details>

<h2 align="center"> Usage </h2>

`abstouch` binary is available after installing.

```bash
$ abstouch <command> [options] [arguments]
```

See help for more information.

```bash
$ abstouch help
```

<h2 align="center"> Setup </h2>

```bash
# Set touchpad area (just draw a rectagle on your touchpad 
# of what area you want to use). Required abstouch restart after calibrating.
$ abstouch calibrate

# Start abstouch
$ abstouch start

# Stop abstouch after you want normal touchpad mode back
$ abstouch stop
```
