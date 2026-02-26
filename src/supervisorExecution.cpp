#include "supervisor_execution.hpp"

#include <functional>

SupervisorExecutionNode::SupervisorExecutionNode()
: Node("supervisor_execution_node")
{
  RCLCPP_INFO(this->get_logger(), "supervisor_execution_node: started");

  input_trigger_pub_ = this->create_publisher<std_msgs::msg::Empty>(
    "/input_trigger", 10);

  execution_result_pub_ = this->create_publisher<std_msgs::msg::String>(
    "/supervisor/supervisor_execution/result", 10);

  allow_next_instruction_sub_ = this->create_subscription<std_msgs::msg::Empty>(
    "/supervisor/supervisor_manager/allow_next_instruction",
    rclcpp::QoS(10),
    std::bind(&SupervisorExecutionNode::allowNextInstructionCallback, this, std::placeholders::_1)
  );

  authorize_task_srv_ = this->create_service<supervisor_executor_interfaces::srv::AuthorizeTask>(
    "/supervisor/supervisor_executor/authorize",
    std::bind(
      &SupervisorExecutionNode::authorizeTaskCallback,
      this,
      std::placeholders::_1,
      std::placeholders::_2
    )
  );
}

void SupervisorExecutionNode::allowNextInstructionCallback(const std_msgs::msg::Empty::SharedPtr msg)
{
  (void)msg;
  RCLCPP_INFO(this->get_logger(), "execution semaphore: allow_next_instruction received");
  publishInputTriggerOnce();
}

void SupervisorExecutionNode::authorizeTaskCallback(
  const supervisor_executor_interfaces::srv::AuthorizeTask::Request::SharedPtr req,
  supervisor_executor_interfaces::srv::AuthorizeTask::Response::SharedPtr res)
{
  res->approved = true;
  res->message = "ok";
  processAuthorizeTask(*req);
}

void SupervisorExecutionNode::processAuthorizeTask(
  const supervisor_executor_interfaces::srv::AuthorizeTask::Request & req)
{
  // Supervisor manager decides what to do next; execution only reports result.
  publishExecutionResult(req.action, req.worker_result);
}

void SupervisorExecutionNode::publishInputTriggerOnce()
{
  input_trigger_pub_->publish(std_msgs::msg::Empty{});
}

void SupervisorExecutionNode::publishExecutionResult(
  const std::string & action,
  const std::string & result)
{
  std_msgs::msg::String msg;
  msg.data = action + ":" + result;
  execution_result_pub_->publish(msg);
  RCLCPP_INFO(
    this->get_logger(),
    "execution result sent to manager: %s",
    msg.data.c_str()
  );
}
