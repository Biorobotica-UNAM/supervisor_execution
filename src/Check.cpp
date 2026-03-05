#include "supervisor_execution/Check.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Check::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Check requires a non-empty argument");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("Check timed out for argument '" + argument + "'");
  }

  if (normalized_result != "success" && normalized_result != "failure")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
      "Check returned unsupported worker_result='" + worker_result + "'");
  }

  const bool is_door_check =
    normalized_argument == "door" ||
    normalized_argument == "door_open" ||
    normalized_argument == "door_closed" ||
    normalized_argument == "open_door" ||
    normalized_argument == "closed_door";

  if (!is_door_check)
  {
    if (normalized_result == "success")
    {
      return makeSuccess("Check succeeded for argument '" + argument + "'");
    }
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Check failed for argument '" + argument + "'");
  }

  const bool expects_open =
    normalized_argument == "door" ||
    normalized_argument == "door_open" ||
    normalized_argument == "open_door";

  if (expects_open)
  {
    if (normalized_result == "success")
    {
      return makeSuccess("Check confirmed door is open");
    }
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_DOOR_CLOSED,
      "Check expected door open, but worker reported closed");
  }

  if (normalized_result == "failure")
  {
    return makeSuccess("Check confirmed door is closed");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_DOOR_OPEN,
    "Check expected door closed, but worker reported open");
}
