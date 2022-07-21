cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_COMPILER "clang")
set(CMAKE_CXX_STANDARD 20)

project(ssfw CXX)

add_subdirectory("vendor/nlohmann_json")

set(ssfw_src
	"src/main.cpp"
	"src/model.cpp"
	"src/simulation.cpp"

	"src/components/component.cpp"
	"src/components/generator.cpp"
	"src/components/router.cpp"
	"src/components/server.cpp"
	"src/components/sink.cpp"
)

add_executable(ssfw ${ssfw_src})

target_link_libraries(ssfw nlohmann_json)

target_include_directories(ssfw PUBLIC "src")
