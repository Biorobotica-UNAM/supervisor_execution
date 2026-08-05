#include "supervisor_execution/Follow.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Follow::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument != "enable" && normalized_argument != "disable")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Follow requires argument 'enable' or 'disable'",
      supervisor_interfaces::msg::TaskExecutionReport::REJECTED);
  }

  if (normalized_argument == "enable" && normalized_result == "tracking_activated")
  {
    return makeSuccess("Follow enabled tracking");
  }

  if (normalized_argument == "disable" && normalized_result == "tracking_deactivated")
  {
    return makeSuccess("Follow disabled tracking");
  }

  if (normalized_result == "tracking_service_unavailable")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_UNSPECIFIED,
      "Follow tracking service unavailable",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  if (normalized_result == "tracking_toggle_failed")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_UNSPECIFIED,
      "Follow tracking toggle failed",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Follow returned unsupported worker_result='" + worker_result + "'",
    supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}
