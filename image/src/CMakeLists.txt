

set(Hbeat_image_SOURCES
	Hbeat_image.cpp
	color-source.cpp
	obs-slideshow.cpp)



add_library(Hbeat_image MODULE
	${Hbeat_image_SOURCES})
target_link_libraries(Hbeat_image
	libobs
	LSL::lsl
	Threads::Threads
	Freetype::Freetype
	${image-source_PLATFORM_DEPS}
	${LIBOBS_LIBRARIES}
	${OBS_FRONTEND_LIBRARIES}
	${lsl-obs-plugin_PLATFORM_DEPS}
	${FREETYPE_LIBRARIES})
