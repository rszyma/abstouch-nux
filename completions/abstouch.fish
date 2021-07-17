#!/usr/bin/env fish
set -l commands help start stop setup calibrate config
complete -c abstouch -f

complete -c abstouch -n "not __fish_seen_subcommand_from $commands" \
    -a 'help' -d 'Shows the help text.'
complete -c abstouch -n "not __fish_seen_subcommand_from $commands" \
    -a 'start' -d 'Starts the abstouch input client.'
complete -c abstouch -n "not __fish_seen_subcommand_from $commands" \
    -a 'stop' -d 'Stops the abstouch input client running as daemon.'
complete -c abstouch -n "not __fish_seen_subcommand_from $commands" \
    -a 'setup' -d 'Runs the abstouch setup.'
complete -c abstouch -n "not __fish_seen_subcommand_from $commands" \
    -a 'calibrate' -d 'Calibrates the abstouch input client.'
complete -c abstouch -n "not __fish_seen_subcommand_from $commands" \
    -a 'config' -d 'Changes or shows the abstouch configuration interactively.'

complete -c abstouch -n '__fish_seen_subcommand_from start' \
    -a '--foreground' -d 'Runs the client on foreground instead of background.'
complete -c abstouch -n '__fish_seen_subcommand_from start' \
    -a '--quiet' -d 'Disables the output with the client except errors.'

complete -c abstouch -n '__fish_seen_subcommand_from calibrate' \
    -a '--no-visual' -d 'Disables the visualization while calibrating.'
