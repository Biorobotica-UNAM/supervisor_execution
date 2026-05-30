#include "supervisor_execution/Check.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Check::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_result == "timeout")
  {
    return makeTimeout("Check timed out for argument '" + argument + "'");
  }

  // The worker now returns explicit machine-readable door outcomes, while
  // keeping success/failure accepted for older integrations.
  const bool door_open =
    normalized_result == "success" ||
    supervisor_execution::string_utils::startsWith(normalized_result, "door_open");

  const bool door_closed =
    normalized_result == "failure" ||
    supervisor_execution::string_utils::startsWith(normalized_result, "door_closed");

  const bool check_service_failed =
    supervisor_execution::string_utils::startsWith(normalized_result, "check_service_failed");

  const bool unsupported_check =
    supervisor_execution::string_utils::startsWith(normalized_result, "unsupported_check");

  const bool check_not_available =
    supervisor_execution::string_utils::startsWith(normalized_result, "check_not_available");

  if (unsupported_check)
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_UNSUPPORTED_ARGUMENT,
      "Check does not support argument '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::REJECTED);
  }

  if (check_not_available)
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_NOT_IMPLEMENTED,
      "Check for argument '" + argument + "' is not available yet",
      supervisor_interfaces::msg::TaskExecutionReport::REJECTED);
  }

  if (!door_open && !door_closed && !check_service_failed)
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
      "Check returned unsupported worker_result='" + worker_result + "'");
  }

  if (check_service_failed)
  {
    // Service failure is a technical execution failure; it must not be treated
    // as a closed door that can be fixed by asking a human to open it.
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_CHECK_SERVICE_FAILED,
      "Check service failed for argument '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  // Door checks require the door to be open before the plan can continue.
  if (door_open)
  {
    return makeSuccess("Check confirmed door is open");
  }

  // Door-closed checks are rejected with SUCCESS status so the manager can run
  // its corrective wait/retry policy instead of classifying this as a hard error.
  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_DOOR_CLOSED,
    "Check expected door open, but worker reported closed");
}