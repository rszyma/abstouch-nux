<h1 align="center">
    abstouch-nux
</h1>
<h3 align="center">
    An absolute touchpad input client for GNU/Linux.
</h3>

<h5 align="center">
    <a href="https://github.com/acedron/abstouch-nux/blob/master/LICENSE"> <img src="https://img.shields.io/github/license/acedron/abstouch-nux?style=for-the-badge" alt="License" /> </a>
    <a href="https://github.com/acedron/abstouch-nux/commit"> <img src="https://img.shields.io/github/last-commit/acedron/abstouch-nux?style=for-the-badge" alt="Last Commit" /> </a>
    <a href="https://github.com/acedron/abstouch-nux/actions"> <img src="https://img.shields.io/github/workflow/status/acedron/abstouch-nux/CMake?style=for-the-badge" alt="Build Status" /> </a>
</h5>


&nbsp;
<h2 align="center"> Installation </h2>

<details>
  <summary><strong> Arch Linux (Manjaro, Artix, etc.) </strong></summary>

The package is available in the [AUR](https://aur.archlinux.org/packages/abstouch-nux).

```bash
$ git clone https://aur.archlinux.org/abstouch-nux.git
$ cd abstouch-nux
$ makepkg -si
```

**Make sure to add the user into the `input` group.**

```bash
$ sudo usermod -aG input $(whoami)
```
</details>

<details>
  <summary><strong> Debian/Ubuntu (Kali, Mint, etc.) </strong></summary>

The package is available in my [ppa](https://launchpad.net/~acedron/+archive/ubuntu/ppa).

```bash
$ echo -e "deb http://ppa.launchpad.net/acedron/ppa/ubuntu focal main\ndeb-src http://ppa.launchpad.net/acedron/ppa/ubuntu focal main" | sudo tee -a /etc/apt/sources.list
$ sudo apt-get update
$ sudo apt-get install abstouch-nux
```

**Make sure to add the user into the `input` group.**

```bash
$ sudo usermod -aG input $(whoami)
```
</details>

<details>
  <summary><strong> Building From Source </strong></summary>

You can make the project from source with [CMake](https://cmake.org).

You should install the dependencies first.
- **Arch Linux**: `$ sudo pacman -Sy cmake gcc libxi libx11 xf86-input-libinput --needed`
- **Debian/Ubuntu**: `$ sudo apt-get install cmake gcc libxi-dev libx11-dev libxi6 libx11-6 xserver-xorg-input-libinput`
- **Fedora/Red Hat**: `$ sudo dnf install cmake gcc libXi-devel libX11-devel libXi libX11 xorg-x11-drv-libinput`
- **openSUSE**: `$ sudo zypper install cmake gcc libXi-devel libX11-devel libXi6 libX11-6 xf86-input-libinput`

Then you can build the package.

```bash
$ git clone https://github.com/acedron/abstouch-nux.git
$ cd abstouch-nux
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

<h2 align="center"> Examples </h2>

```bash
# Setup the abstouch-nux client first.
$ abstouch setup

# Calibrate the abstouch-nux client.
$ abstouch calibrate

# Start the abstouch-nux client on foreground.
$ abstouch start -f

# Start the abstouch-nux client as daemon.
$ abstouch start

# Stop the abstouch-nux client quietly.
$ abstouch stop --quiet

# Recalibrate the client, without the visualization.
$ abstouch calibrate --no-visual
```
