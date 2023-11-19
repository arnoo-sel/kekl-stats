cmake_minimum_required(VERSION 3.14)

project(qttm LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)
find_package(QCoro6 REQUIRED COMPONENTS Core Network)

add_executable(qttm
  src/main.cpp
  src/qttm.hpp
  src/auth.hpp                     src/auth.cpp
  src/examples.hpp                 src/examples.cpp
  src/requestqueue.hpp             src/requestqueue.cpp
  src/utils.hpp                    src/utils.cpp
  src/api/core/core.hpp            src/api/core/core.cpp
  src/api/core/records.hpp         src/api/core/records.cpp
  src/api/live/clubs.hpp           src/api/live/clubs.cpp
  src/api/live/live.hpp            src/api/live/live.cpp
  src/api/live/leaderboards.hpp    src/api/live/leaderboards.cpp
  src/api/live/maps.hpp            src/api/live/maps.cpp
  src/api/oauth/accounts.hpp       src/api/oauth/accounts.cpp
  src/api/oauth/oauth.hpp          src/api/oauth/oauth.cpp
  src/exception/jsonparseerror.hpp src/exception/jsonparseerror.cpp
  src/exception/networkerror.hpp   src/exception/networkerror.cpp
  src/exception/qttmexception.hpp  src/exception/qttmexception.cpp
)

target_link_libraries(qttm Qt6::Core Qt6::Network QCoro::Core QCoro::Network)

include(GNUInstallDirs)
install(TARGETS qttm
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
