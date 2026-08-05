#include "supervisor_execution/VerifyTake.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult VerifyTake::validate(
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
      "VerifyTake requires a non-empty object argument");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("VerifyTake timed out for argument '" + argument + "'");
  }

  if (supervisor_execution::string_utils::startsWith(
      normalized_result,
      "take_verified:"))
  {
    return makeSuccess("Take verified for argument '" + argument + "'");
  }

  if (supervisor_execution::string_utils::startsWith(
      normalized_result,
      "take_failed_object_still_present:"))
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "VerifyTake found object still present for argument '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  if (supervisor_execution::string_utils::startsWith(
      normalized_result,
      "take_uncertain:"))
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_UNSPECIFIED,
      "VerifyTake was uncertain for argument '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  if (normalized_result == "invalid_argument")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "VerifyTake received invalid argument '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::REJECTED);
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "VerifyTake returned unsupported worker_result='" + worker_result + "'",
    supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}
