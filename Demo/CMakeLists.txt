cmake_minimum_required(VERSION 3.20)


add_executable(Demo)

target_sources(Demo
	PRIVATE
		DemoMain.cpp
)

target_include_directories(Demo PRIVATE ${DemoDir})

# TODO: Demo depends on test utilities - Inversion of dependency!
target_link_libraries(Demo PRIVATE curlew)

sequoia_finalize_executable(Demo)