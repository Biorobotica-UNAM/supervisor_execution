#include <memory>

#include "supervisor_execution.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SupervisorExecutionNode>());
  rclcpp::shutdown();
  return 0;
}
