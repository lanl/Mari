cmake_minimum_required(VERSION 3.16)
project(qtest_hked LANGUAGES CXX)

set(Qt6_DIR "C:/Qt/6.9.3/msvc2022_64/lib/cmake/Qt6")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport Test)

# Good defaults (turns on AUTOMOC/AUTOUIC/AUTORCC per target)
qt_standard_project_setup()
set(CMAKE_AUTOGEN_VERBOSE ON)  # <-- helps you confirm moc runs

# ----- Library under test -----------------------------------------------------
qt_add_library(hked STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../Qt/src/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Qt/include/loadspectrumdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../Qt/include/okbutton.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../Qt/include/cancelbutton.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../Qt/include/browsebutton.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/kvcomponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/kvmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/kvaction.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/kvkedplotstate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/kvxrfplotstate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/kkedplotmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/kxrfplotmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/krunmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/kfilemanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/kanalysismanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/kdevlogger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/kaction1.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/kaction2.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../Qt/include/mainwindow.h  # must contain Q_OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/../Qt/src/loadspectrumdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Qt/src/okbutton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Qt/src/cancelbutton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Qt/src/browsebutton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/QCustomPlot/qcustomplot.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/kvcomponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/kvmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/kvaction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/kvkedplotstate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/kvxrfplotstate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/kkedplotmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/kxrfplotmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/krunmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/kfilemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/ksnalysismanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/kdevlogger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/kaction1.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/kaction2.cpp
)

target_include_directories(hked PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/QCustomPlot
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/SpecUtils
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/SpecUtils/SpecUtils
    ${CMAKE_CURRENT_SOURCE_DIR}/../Qt/include
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/proj_build)

# Link Qt *used by the library* here (NOT Qt::Test)
target_link_libraries(hked PUBLIC
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::PrintSupport
)

# Since the library depends on SpecUtils via add_subdirectory, bring it in BEFORE linking:
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/SpecUtils ${CMAKE_BINARY_DIR}/proj_build)

# And link it if the hked sources include its headers or symbols:
target_link_libraries(hked PUBLIC SpecUtils)

# ----- Test executable --------------------------------------------------------
# Use qt_add_executable so AUTOGEN/moc runs for the test class too
qt_add_executable(Test_MainWindow
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Test_MainWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Test_MainWindow.h
)

# The test exe links your lib + Qt Test
target_link_libraries(Test_MainWindow PRIVATE hked Qt6::Test)

# ----- CTest wiring -----------------------------------------------------------
include(CTest)
enable_testing()
add_test(NAME mainwindow_test COMMAND Test_MainWindow)
#add_test(NAME FileManager_test COMMAND Test_FileManager)