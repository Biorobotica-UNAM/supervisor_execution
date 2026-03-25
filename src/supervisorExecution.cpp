#include "supervisor_execution.hpp"

#include <functional>

#include "supervisor_execution/StringUtils.hpp"

SupervisorExecutionNode::SupervisorExecutionNode()
: Node("supervisor_execution_node")
{
  RCLCPP_INFO(this->get_logger(), "supervisor_execution_node: started");

  input_trigger_pub_ = this->create_publisher<std_msgs::msg::Empty>(
    "/input_trigger", 10);

  execution_result_pub_ = this->create_publisher<supervisor_interfaces::msg::TaskExecutionReport>(
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
  const auto validation = processAuthorizeTask(*req);

  res->approved = validation.approved;
  if (validation.approved) {
    res->message = "approved";
  } else if (!validation.failure_reason.empty()) {
    res->message = validation.failure_reason;
  } else {
    res->message = "rejected";
  }

  publishExecutionResult(*req, validation);
}

ActionValidationResult SupervisorExecutionNode::processAuthorizeTask(
  const supervisor_executor_interfaces::srv::AuthorizeTask::Request & req) const
{
  const std::string action =
    supervisor_execution::string_utils::normalize(req.action);

  if (action == "goto" || action == "go_to")
  {
    return goto_authorizer_.validate(req.argument, req.worker_result);
  }

  if (action == "say")
  {
    return say_authorizer_.validate(req.argument, req.worker_result);
  }

  if (action == "listen" || action == "listen_for")
  {
    return listen_authorizer_.validate(req.argument, req.worker_result);
  }

  if (action == "check" || action == "wait_for")
  {
    return check_authorizer_.validate(req.argument, req.worker_result);
  }

  if(action == "save")
  {
    return save_authorizer_.validate(req.argument, req.worker_result);
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_UNKNOWN_ACTION,
    "Unsupported action '" + req.action + "'",
    supervisor_interfaces::msg::TaskExecutionReport::SUCCESS);
}

void SupervisorExecutionNode::publishInputTriggerOnce()
{
  input_trigger_pub_->publish(std_msgs::msg::Empty{});
}

void SupervisorExecutionNode::publishExecutionResult(
  const supervisor_executor_interfaces::srv::AuthorizeTask::Request & req,
  const ActionValidationResult & validation)
{
  supervisor_interfaces::msg::TaskExecutionReport msg;
  msg.action = req.action;
  msg.argument = req.argument;
  msg.worker_result = req.worker_result;
  msg.context_message = req.context_message;
  msg.approved = validation.approved;
  msg.status = validation.status;
  msg.failure_reason = validation.failure_reason;
  msg.details = validation.details;

  execution_result_pub_->publish(msg);

  RCLCPP_INFO(
    this->get_logger(),
    "execution result action='%s' arg='%s' worker='%s' approved=%s status=%u reason='%s'",
    msg.action.c_str(),
    msg.argument.c_str(),
    msg.worker_result.c_str(),
    msg.approved ? "true" : "false",
    static_cast<unsigned int>(msg.status),
    msg.failure_reason.c_str()
  );
}
