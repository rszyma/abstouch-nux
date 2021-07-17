enable_testing()

cmake_minimum_required(VERSION 3.9)
project(abstouch-nux LANGUAGES C VERSION 1.0.0 DESCRIPTION "An absolute touchpad input client for GNU/Linux.")
include(GNUInstallDirs)

set(CMAKE_C_FLAGS "-Wno-format-security")
set(CMAKE_CXX_FLAGS "-Wno-format-security")

list(APPEND sources src/abstouch.c src/config.c src/getch.c src/print.c)
list(APPEND sources src/linux/event.c src/linux/client.c src/linux/display.c)
list(APPEND libraries -lm)
list(APPEND libraries -lX11 -lXi)

add_executable(abstouch ${sources})
target_link_libraries(abstouch ${libraries})
set_target_properties(abstouch PROPERTIES VERSION ${PROJECT_VERSION})

add_test(NAME Test COMMAND abstouch help)

install(TARGETS abstouch DESTINATION ${CMAKE_INSTALL_BINDIR})
if (UNIX)
    install(FILES completions/abstouch DESTINATION ${CMAKE_INSTALL_PREFIX}/share/bash-completion/completions)
    install(FILES completions/_abstouch DESTINATION ${CMAKE_INSTALL_PREFIX}/share/zsh/site-functions)
    install(FILES completions/abstouch.fish DESTINATION ${CMAKE_INSTALL_PREFIX}/share/fish/vendor_completions.d)
    install(FILES doc/abstouch.8 DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man8)
    install(FILES doc/abstouch-nux.8 DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man8)
endif (UNIX)
