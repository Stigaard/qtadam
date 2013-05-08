cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED QtNetwork)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
set(qtADAM_SRCS qadam.cpp)
qt4_automoc(${qtADAM_SRCS})
ADD_LIBRARY(qtADAM STATIC ${qtADAM_SRCS})
target_link_libraries(qtADAM ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY})
