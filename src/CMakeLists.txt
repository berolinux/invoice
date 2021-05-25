# SPDX-License-Identifier: GPL-2.0-or-later
# (C) 2022 Bernhard Rosenkränzer <bero@lindev.ch>
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_executable(invoice invoice.cpp)
target_link_libraries(invoice Qt6::Core Qt6::Gui)

add_executable(invoice-config invoice-config.cpp ConfigDialog.cpp)
target_link_libraries(invoice-config Qt6::Core Qt6::Gui Qt6::Widgets)

install(TARGETS invoice invoice-config DESTINATION bin)
