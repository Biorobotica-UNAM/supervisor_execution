#include "supervisor_execution/GoTo.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult GoTo::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "GoTo requires a non-empty destination argument");
  }

  if (normalized_result == "success")
  {
    return makeSuccess("GoTo reached destination '" + argument + "'");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("GoTo timed out while moving to '" + argument + "'");
  }

  if (normalized_result == "failure")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_NAVIGATION_FAILED,
      "GoTo failed while moving to '" + argument + "'");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "GoTo returned unsupported worker_result='" + worker_result + "'");
}
