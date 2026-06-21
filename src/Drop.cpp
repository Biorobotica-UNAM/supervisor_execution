#include "supervisor_execution/Drop.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Drop::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument =
    supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result =
    supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Drop requires a non-empty target argument");
  }

  if (normalized_result == "success" ||
      normalized_result == "floor_drop_completed" ||
      normalized_result == "left_arm_group_drop_completed" ||
      normalized_result == "right_arm_group_drop_completed")
  {
    return makeSuccess("Drop completed for argument '" + argument + "'");
  }

  if (normalized_result == "timeout" ||
      normalized_result.find("timeout") != std::string::npos)
  {
    return makeTimeout("Drop timed out for argument '" + argument + "'");
  }

  if (normalized_result.find("goal_cancelled") != std::string::npos)
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Drop goal was cancelled for argument '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Drop returned unsupported worker_result='" + worker_result + "'",
    supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}
