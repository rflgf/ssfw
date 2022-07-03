cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_COMPILER "clang")
set(CMAKE_CXX_STANDARD 20)

project(ssfw CXX)

set(CMAKE_CXX_FLAGS "") 

set(ssfw_src
	"src/main.cpp"
	"src/model.cpp"
	"src/simulation.cpp"

	"src/components/component.cpp"
	"src/components/input.cpp"
	"src/components/output.cpp"
	"src/components/router.cpp"
	"src/components/service_center.cpp"
)

add_executable(ssfw ${ssfw_src})

target_include_directories(ssfw PUBLIC "src")