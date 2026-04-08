#ifndef SUPERVISOR_EXECUTION_HPP_
#define SUPERVISOR_EXECUTION_HPP_

#include <rclcpp/rclcpp.hpp>

#include <string>

#include <std_msgs/msg/empty.hpp>

#include <supervisor_executor_interfaces/srv/authorize_task.hpp>
#include <supervisor_interfaces/msg/task_execution_report.hpp>

#include "supervisor_execution/ActionValidationResult.hpp"
#include "supervisor_execution/Check.hpp"
#include "supervisor_execution/GoTo.hpp"
#include "supervisor_execution/Listen.hpp"
#include "supervisor_execution/Say.hpp"
#include "supervisor_execution/Save.hpp"

class SupervisorExecutionNode : public rclcpp::Node
{
public:
  SupervisorExecutionNode();

private:
  void authorizeTaskCallback(
    const supervisor_executor_interfaces::srv::AuthorizeTask::Request::SharedPtr req,
    supervisor_executor_interfaces::srv::AuthorizeTask::Response::SharedPtr res);

  void allowNextInstructionCallback(const std_msgs::msg::Empty::SharedPtr msg);

  ActionValidationResult processAuthorizeTask(
    const supervisor_executor_interfaces::srv::AuthorizeTask::Request & req) const;

  void publishInputTriggerOnce();
  void publishExecutionResult(
    const supervisor_executor_interfaces::srv::AuthorizeTask::Request & req,
    const ActionValidationResult & validation);

  rclcpp::Publisher<std_msgs::msg::Empty>::SharedPtr input_trigger_pub_;
  rclcpp::Publisher<supervisor_interfaces::msg::TaskExecutionReport>::SharedPtr execution_result_pub_;

  rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr allow_next_instruction_sub_;
  rclcpp::Service<supervisor_executor_interfaces::srv::AuthorizeTask>::SharedPtr authorize_task_srv_;

  GoTo goto_authorizer_;
  Say say_authorizer_;
  Listen listen_authorizer_;
  Check check_authorizer_;
  Save save_authorizer_;
};

#endif  // SUPERVISOR_EXECUTION_HPP_
