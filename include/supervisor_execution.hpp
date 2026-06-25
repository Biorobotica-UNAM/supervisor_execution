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
#include "supervisor_execution/Describe.hpp"
#include "supervisor_execution/Point.hpp"
#include "supervisor_execution/Find.hpp"
#include "supervisor_execution/Focus.hpp"
#include "supervisor_execution/Follow.hpp"
#include "supervisor_execution/Unfocus.hpp"
#include "supervisor_execution/MemorizeFace.hpp"
#include "supervisor_execution/SaveLoc.hpp"
#include "supervisor_execution/Align.hpp"
#include "supervisor_execution/Take.hpp"
#include "supervisor_execution/Fold.hpp"
#include "supervisor_execution/Drop.hpp"
#include "supervisor_execution/Analyze.hpp"
#include "supervisor_execution/Approach.hpp"

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
  void scheduleInputTriggerRetry();
  void publishExecutionResult(
    const supervisor_executor_interfaces::srv::AuthorizeTask::Request & req,
    const ActionValidationResult & validation);

  rclcpp::Publisher<std_msgs::msg::Empty>::SharedPtr input_trigger_pub_;
  rclcpp::Publisher<supervisor_interfaces::msg::TaskExecutionReport>::SharedPtr execution_result_pub_;

  rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr allow_next_instruction_sub_;
  rclcpp::Service<supervisor_executor_interfaces::srv::AuthorizeTask>::SharedPtr authorize_task_srv_;
  // Reintenta el pulso /input_trigger cuando executor_manager arranca tarde.
  rclcpp::TimerBase::SharedPtr input_trigger_retry_timer_;

  GoTo          goto_authorizer_;
  Say           say_authorizer_;
  Listen        listen_authorizer_;
  Check         check_authorizer_;
  Save          save_authorizer_;
  Describe      describe_authorizer_;
  Point         point_authorizer_;
  Find          find_authorizer_;
  Focus         focus_authorizer_;
  Follow        follow_authorizer_;
  Unfocus       unfocus_authorizer_;
  MemorizeFace  memorize_face_authorizer_;
  SaveLoc       saveloc_authorizer_;
  Align         align_authorizer_;
  Take          take_authorizer_;
  Drop          drop_authorizer_;
  Fold          fold_authorizer_;
  Analyze       analyze_authorizer_;
  Approach      approach_authorizer_;
};

#endif  // SUPERVISOR_EXECUTION_HPP_
