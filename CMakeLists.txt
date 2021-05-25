# SPDX-License-Identifier: GPL-2.0-or-later
# (C) 2022 Bernhard Rosenkränzer <bero@lindev.ch>
cmake_minimum_required(VERSION 3.18.2)
project(invoice VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 6.1.0 CONFIG REQUIRED COMPONENTS Core Gui Widgets)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f00)

find_package(PkgConfig REQUIRED)

# FIXME why doesn't anything set QT_INCLUDES?
message("${QT6_INCLUDES} ${QT_INCLUDES}")
include_directories(/usr/include/qt6 /usr/include/qt6/QtCore /usr/include/qt6/QtGui /usr/include/qt6/QtWidgets)

add_subdirectory(src)
add_subdirectory(generators)
