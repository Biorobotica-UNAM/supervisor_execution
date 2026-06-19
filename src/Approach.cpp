#include "supervisor_execution/Approach.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Approach::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Approach requires a non-empty target argument",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  if (normalized_result == "success" ||
      normalized_result == "arrived_by_tf_fallback" ||
      normalized_result == "partial_goal_already_close" ||
      normalized_result == "partial_arrived_by_tf_fallback" ||
      supervisor_execution::string_utils::startsWith(
        normalized_result, "navigation_status=succeeded") ||
      supervisor_execution::string_utils::startsWith(
        normalized_result, "partial_navigation_status=succeeded"))
  {
    return makeSuccess("Approach completed for argument '" + argument + "'");
  }

  if (normalized_result == "timeout" ||
      normalized_result == "navigation_timeout" ||
      normalized_result.find("timeout") != std::string::npos)
  {
    return makeTimeout("Approach timed out for argument '" + argument + "'");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_NAVIGATION_FAILED,
    "Approach failed for argument '" + argument + "': " + worker_result,
    supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}
