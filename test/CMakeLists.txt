project(libqofono-tests LANGUAGES CXX)

# TODO: disable tests if these aren't found.
find_program(DBUS_RUNNER dbus-test-runner)
find_program(OFONO_DAEMON ofonod)
IF (DBUS_RUNNER AND OFONO_DAEMON)
  find_program(PHONESIM ofono-phonesim)
ENDIF(DBUS_RUNNER AND OFONO_DAEMON)

add_subdirectory(auto/tst_qofono)
add_subdirectory(auto/tests)
# add_subdirectory(auto/tst_location)
