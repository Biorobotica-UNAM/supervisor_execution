cmake_minimum_required(VERSION 3.8)
project(supervisor_execution)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(supervisor_executor_interfaces REQUIRED)
find_package(supervisor_interfaces REQUIRED)

add_executable(supervisor_execution_node
  src/supervisor_execution_node.cpp
  src/supervisorExecution.cpp
  src/GoTo.cpp
  src/Say.cpp
  src/Listen.cpp
  src/Check.cpp
  src/Save.cpp
  src/Find.cpp
  src/Focus.cpp
  src/Follow.cpp
  src/Unfocus.cpp
  src/MemorizeFace.cpp
  src/Describe.cpp
  src/Point.cpp
  src/SaveLoc.cpp
  src/Approach.cpp
  src/Align.cpp
  src/Take.cpp
  src/Analyze
)

target_include_directories(supervisor_execution_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(supervisor_execution_node
  rclcpp
  std_msgs
  supervisor_executor_interfaces
  supervisor_interfaces
)

install(TARGETS supervisor_execution_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
