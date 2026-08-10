# Zephyr adds ${shield_dir}/CMakeLists.txt for out-of-tree shields too
# (see zephyr/boards/shields/CMakeLists.txt), so the custom status screen can
# live next to the shield -- no separate module needed.

if(CONFIG_ZMK_DISPLAY AND CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM)
  zephyr_library_named(alixw_display)
  # ZMK's app/include is PRIVATE to the app target, so pull it in explicitly
  # to get <zmk/display.h>, <zmk/endpoints.h> etc.
  zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include)
  zephyr_library_sources(custom_status_screen.c)
  zephyr_library_sources(widgets/output_status.c)
endif()
