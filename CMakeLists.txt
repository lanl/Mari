cmake_minimum_required(VERSION 3.16)
project(HKED VERSION 0.1 LANGUAGES CXX)

# ------------------------------------------------------------
# Let the user decide the tool‑chain (e.g. Vcpkg) via -D
# ------------------------------------------------------------
# Nothing to write here – the tool‑chain file will be supplied
# from the command line or IDE.

# ------------------------------------------------------------
# General compiler options
# ------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------
# Locate Qt 
# ------------------------------------------------------------
set(Qt6_DIR "C:/Qt/6.9.3/msvc2022_64/lib/cmake/Qt6")   # <-- optional, you could expose this too
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)

# ------------------------------------------------------------
# Packages that come from Vcpkg (or any other manager)
# ------------------------------------------------------------
# The tool‑chain file already added the Vcpkg prefix to CMAKE_PREFIX_PATH,
# so simple CONFIG finds work.
find_package(gflags CONFIG REQUIRED)   # needed by glog → ceres
find_package(glog   CONFIG REQUIRED)   # optional – Ceres pulls it in anyway
find_package(Ceres  CONFIG REQUIRED)   # pulls glog & gflags transitively
find_package(nlohmann_json CONFIG REQUIRED)

# ------------------------------------------------------------
# Qt convenience helpers
# ------------------------------------------------------------
qt_standard_project_setup()
set(CMAKE_AUTOUIC OFF)   # you manually handle UI files
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON) 

# ------------------------------------------------------------
# Sources / headers 
# ------------------------------------------------------------
set(headers 
  #GUI classes
  ${PROJECT_SOURCE_DIR}/Qt/include/mainwindow.h
  ${PROJECT_SOURCE_DIR}/Qt/include/loadspectrumdialog.h
  ${PROJECT_SOURCE_DIR}/Qt/include/analyzedialog.h
  ${PROJECT_SOURCE_DIR}/Qt/include/calibratekeddialog.h
  ${PROJECT_SOURCE_DIR}/Qt/include/calibratexrfdialog.h
  ${PROJECT_SOURCE_DIR}/Qt/include/calibratexrfresultsdialog.h
  ${PROJECT_SOURCE_DIR}/Qt/include/newparameterdialog.h
  ${PROJECT_SOURCE_DIR}/Qt/include/editparameterdialog.h
  ${PROJECT_SOURCE_DIR}/Qt/include/deleteparameterdialog.h
  ${PROJECT_SOURCE_DIR}/Qt/include/okbutton.h
  ${PROJECT_SOURCE_DIR}/Qt/include/cancelbutton.h
  ${PROJECT_SOURCE_DIR}/Qt/include/browsebutton.h
  ${PROJECT_SOURCE_DIR}/Qt/include/deletebutton.h
  ${PROJECT_SOURCE_DIR}/Qt/include/questionbutton.h
  ${PROJECT_SOURCE_DIR}/Qt/include/updatablelabel.h
  ${PROJECT_SOURCE_DIR}/Qt/include/nonupdatablelabel.h
  ${PROJECT_SOURCE_DIR}/Qt/include/custommsgbox.h
  #Externals
  #${PROJECT_SOURCE_DIR}/external/QCustomPlot/qcustomplot.h
  #Manager classes
  ${PROJECT_SOURCE_DIR}/include/kvcomponent.h
  ${PROJECT_SOURCE_DIR}/include/kvmanager.h
  ${PROJECT_SOURCE_DIR}/include/kkedplotmanager.h
  ${PROJECT_SOURCE_DIR}/include/kxrfplotmanager.h
  ${PROJECT_SOURCE_DIR}/include/krunmanager.h
  ${PROJECT_SOURCE_DIR}/include/kfilemanager.h
  ${PROJECT_SOURCE_DIR}/include/kresultsmanager.h
  ${PROJECT_SOURCE_DIR}/include/kdevlogger.h
  #Action classes
  ${PROJECT_SOURCE_DIR}/include/kvaction.h
  ${PROJECT_SOURCE_DIR}/include/kmath.h
  ${PROJECT_SOURCE_DIR}/include/kvkedplotstate.h
  ${PROJECT_SOURCE_DIR}/include/kvxrfplotstate.h
  ${PROJECT_SOURCE_DIR}/include/kenergycalibration.h
  ${PROJECT_SOURCE_DIR}/include/kfwhmcalibration.h
  ${PROJECT_SOURCE_DIR}/include/kanalysisworker.h
  ${PROJECT_SOURCE_DIR}/include/kcalibrateworker.h
  ${PROJECT_SOURCE_DIR}/include/kkedengine.h
  ${PROJECT_SOURCE_DIR}/include/kxrfengine.h
  ${PROJECT_SOURCE_DIR}/include/kcalibrateengine.h
  ${PROJECT_SOURCE_DIR}/include/kkedsourceterm.h
  ${PROJECT_SOURCE_DIR}/include/kxrfsourceterm.h
  ${PROJECT_SOURCE_DIR}/include/kcalkedsourceterm.h
  ${PROJECT_SOURCE_DIR}/include/kcalxrfsourceterm.h
  ${PROJECT_SOURCE_DIR}/include/kenergycalibrationprior.h
  #POD classes
  ${PROJECT_SOURCE_DIR}/include/kspectraldata.h
  ${PROJECT_SOURCE_DIR}/include/katomicdata.h
  ${PROJECT_SOURCE_DIR}/include/kkedresults.h
  ${PROJECT_SOURCE_DIR}/include/kxrfresults.h
  ${PROJECT_SOURCE_DIR}/include/kceresresults.h
  ${PROJECT_SOURCE_DIR}/include/kresultssummary.h
  ${PROJECT_SOURCE_DIR}/include/kcalibratekedresults.h
  ${PROJECT_SOURCE_DIR}/include/kcalibratexrfresults.h
)

set(sources
  #GUI classes
  ${PROJECT_SOURCE_DIR}/Qt/src/mainwindow.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/loadspectrumdialog.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/analyzedialog.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/calibratekeddialog.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/calibratexrfdialog.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/calibratexrfresultsdialog.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/newparameterdialog.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/editparameterdialog.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/deleteparameterdialog.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/okbutton.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/cancelbutton.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/browsebutton.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/deletebutton.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/questionbutton.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/updatablelabel.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/nonupdatablelabel.cpp
  ${PROJECT_SOURCE_DIR}/Qt/src/custommsgbox.cpp
  ${PROJECT_SOURCE_DIR}/resources.qrc
  #Externals
  ${PROJECT_SOURCE_DIR}/external/QCustomPlot/qcustomplot.cpp
  #Manager classes
  ${PROJECT_SOURCE_DIR}/src/kvcomponent.cpp
  ${PROJECT_SOURCE_DIR}/src/kvmanager.cpp
  ${PROJECT_SOURCE_DIR}/src/kkedplotmanager.cpp
  ${PROJECT_SOURCE_DIR}/src/kxrfplotmanager.cpp
  ${PROJECT_SOURCE_DIR}/src/krunmanager.cpp
  ${PROJECT_SOURCE_DIR}/src/kfilemanager.cpp
  ${PROJECT_SOURCE_DIR}/src/kresultsmanager.cpp
  ${PROJECT_SOURCE_DIR}/src/kdevlogger.cpp
  #Action classes
  ${PROJECT_SOURCE_DIR}/src/kvaction.cpp
  ${PROJECT_SOURCE_DIR}/src/kmath.cpp
  ${PROJECT_SOURCE_DIR}/src/kvkedplotstate.cpp
  ${PROJECT_SOURCE_DIR}/src/kvxrfplotstate.cpp
  ${PROJECT_SOURCE_DIR}/src/kenergycalibration.cpp
  ${PROJECT_SOURCE_DIR}/src/kfwhmcalibration.cpp
  ${PROJECT_SOURCE_DIR}/src/kanalysisworker.cpp
  ${PROJECT_SOURCE_DIR}/src/kcalibrateworker.cpp
  ${PROJECT_SOURCE_DIR}/src/kkedengine.cpp
  ${PROJECT_SOURCE_DIR}/src/kxrfengine.cpp
  ${PROJECT_SOURCE_DIR}/src/kcalibrateengine.cpp
  ${PROJECT_SOURCE_DIR}/src/kkedsourceterm.cpp
  ${PROJECT_SOURCE_DIR}/src/kxrfsourceterm.cpp
  ${PROJECT_SOURCE_DIR}/src/kcalkedsourceterm.cpp
  ${PROJECT_SOURCE_DIR}/src/kcalxrfsourceterm.cpp
  #POD classes
  ${PROJECT_SOURCE_DIR}/src/kspectraldata.cpp
  ${PROJECT_SOURCE_DIR}/src/kkedresults.cpp
  ${PROJECT_SOURCE_DIR}/src/kxrfresults.cpp
  ${PROJECT_SOURCE_DIR}/src/kceresresults.cpp
  ${PROJECT_SOURCE_DIR}/src/kresultssummary.cpp
  ${PROJECT_SOURCE_DIR}/src/kcalibratekedresults.cpp
  ${PROJECT_SOURCE_DIR}/src/kcalibratexrfresults.cpp
)

# ------------------------------------------------------------
# Include directories 
# ------------------------------------------------------------
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/Qt/include
    ${PROJECT_SOURCE_DIR}/external/QCustomPlot
)

# ------------------------------------------------------------
# External sub‑project (SpecUtils)
# ------------------------------------------------------------
add_subdirectory(${PROJECT_SOURCE_DIR}/external/SpecUtils)

# ------------------------------------------------------------
#  Build the executable
# ------------------------------------------------------------
qt_add_executable(Mari main.cpp ${headers} ${sources})

set_target_properties(Mari PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE     TRUE
)

target_link_libraries(Mari PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
    SpecUtils
    Ceres::ceres          # pulls glog & gflags automatically
    nlohmann_json::nlohmann_json
)

install(TARGETS Mari
    RUNTIME DESTINATION "C:/Users/389692/repos/QtMyApp1App/Working"
    LIBRARY DESTINATION "C:/Users/389692/repos/QtMyApp1App/Working"
)

target_compile_definitions(Mari PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
