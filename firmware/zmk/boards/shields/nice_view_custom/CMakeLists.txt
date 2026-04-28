target_sources(app PRIVATE custom_status_screen.c)
target_sources(app PRIVATE widgets/custom_widgets.c)

zephyr_include_directories(.)
zephyr_include_directories(widgets)
