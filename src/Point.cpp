#include "supervisor_execution/Point.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Point::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Point requires a non-empty action argument");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("Point timed out for argument '" + argument + "'");
  }

  if (normalized_result == "success" ||
      normalized_result == "pointing_enable" ||
      normalized_result == "pointing_enabled" ||
      normalized_result == "pointing_disable" ||
      normalized_result == "pointing_disabled")
  {
    return makeSuccess("Point completed for argument '" + argument + "'");
  }

  if (normalized_result == "failure" ||
      normalized_result == "arm_failed" ||
      normalized_result == "arm_failed_during_pointing" ||
      normalized_result == "arm_failed_during_reset" ||
      normalized_result.rfind("unknown_argument:", 0U) == 0U)
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Point failed for argument '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Point returned unsupported worker_result='" + worker_result + "'",
    supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}
