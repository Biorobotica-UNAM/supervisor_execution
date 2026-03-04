#ifndef SUPERVISOR_EXECUTION_HPP_
#define SUPERVISOR_EXECUTION_HPP_

#include <rclcpp/rclcpp.hpp>

#include <string>

#include <std_msgs/msg/empty.hpp>
#include <std_msgs/msg/string.hpp>

#include <supervisor_executor_interfaces/srv/authorize_task.hpp>

class SupervisorExecutionNode : public rclcpp::Node
{
public:
  SupervisorExecutionNode();

private:
  void authorizeTaskCallback(
    const supervisor_executor_interfaces::srv::AuthorizeTask::Request::SharedPtr req,
    supervisor_executor_interfaces::srv::AuthorizeTask::Response::SharedPtr res);

  void allowNextInstructionCallback(const std_msgs::msg::Empty::SharedPtr msg);

  void processAuthorizeTask(
    const supervisor_executor_interfaces::srv::AuthorizeTask::Request & req);

  void publishInputTriggerOnce();
  void publishExecutionResult(const std::string & action, const std::string & result);

  rclcpp::Publisher<std_msgs::msg::Empty>::SharedPtr input_trigger_pub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr execution_result_pub_;

  rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr allow_next_instruction_sub_;
  rclcpp::Service<supervisor_executor_interfaces::srv::AuthorizeTask>::SharedPtr authorize_task_srv_;
};

#endif  // SUPERVISOR_EXECUTION_HPP_
