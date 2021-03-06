include(ECMMarkAsTest)

find_package(KF5XmlGui ${KF5_DEP_VERSION} REQUIRED)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)

macro(KIOWIDGETS_EXECUTABLE_TESTS)
  foreach(_testname ${ARGN})
    add_executable(${_testname} ${_testname}.cpp)
    target_link_libraries(${_testname}
       KF5::KIOCore
       KF5::KIOWidgets
       KF5::KIOFileWidgets
       Qt5::Test
       KF5::WidgetsAddons
       KF5::IconThemes)
    ecm_mark_as_test(${_testname})
  endforeach()
endmacro(KIOWIDGETS_EXECUTABLE_TESTS)

KIOWIDGETS_EXECUTABLE_TESTS(
  getalltest
  kruntest
  kioslavetest
  kopenwithtest
  kencodingfiledialogtest_gui
  kdirmodeltest_gui
  kdirlistertest_gui
  kfilecustomdialogtest_gui
  previewtest
  kurlrequestertest_gui
  kpropertiesdialogtest
  kmountpoint_debug
  listjobtest
  listrecursivetest
  kionetrctest
  ksycocaupdatetest
  udsentrybenchmark
  kurlnavigatortest_gui
  kprotocolinfo_dumper
  kfilewidgettest_gui
  runapplication
)
