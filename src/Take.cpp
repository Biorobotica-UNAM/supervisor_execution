#include "supervisor_execution/Take.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Take::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Take requires a non-empty action argument. Specify the object to take.");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("Take timed out for argument '" + argument + "'");
  }

  if (normalized_result == "success" || normalized_result == "left_arm_group_take_completed" ||
        normalized_result == "right_arm_group_take_completed")
  {
    return makeSuccess("Take completed for argument '" + argument + "'");
  }

  if (normalized_result == "failure" || normalized_result == "invalid_argument" ||
        normalized_result == "invalid_manipulation_data")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Take failed for argument '" + argument + "'");
  }

  if (normalized_result == "failure" || normalized_result == "invalid_manipulation_pose")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Take failed parsing json pose for argument '" + argument + "'");
  }

  if (normalized_result == "failure" || normalized_result == "gripper_close_failed")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
      "Take failed closing gripper");
  }

  // if (normalized_result.rfind("found:", 0) == 0)
  // {
  //     return makeSuccess("Take completed for argument '" + argument + "'");
  // }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Take returned unsupported worker_result='" + worker_result + "'");
}
