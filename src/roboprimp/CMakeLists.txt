cmake_minimum_required(VERSION 3.20)
project(roboprimp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rclcpp REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/RobotStatus.msg"
  "srv/SetStatus.srv"
  "srv/SetPosition.srv"
  "srv/SetBattery.srv"
)

rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")

add_executable(status_monitor src/status_monitor.cpp)
add_executable(status_hub src/status_hub.cpp)

target_include_directories(status_monitor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_compile_features(status_monitor PUBLIC c_std_17 cxx_std_20)  # Require C17 and C++20
target_link_libraries(status_monitor PUBLIC rclcpp::rclcpp "${cpp_typesupport_target}")
target_link_libraries(status_hub PUBLIC rclcpp::rclcpp "${std_msgs_TARGETS}" "${cpp_typesupport_target}")

install(TARGETS status_monitor status_hub
  DESTINATION lib/${PROJECT_NAME})

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
