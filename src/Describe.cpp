#include "supervisor_execution/Describe.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Describe::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Describe requires a non-empty action argument");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("Describe timed out for argument '" + argument + "'");
  }

  if (normalized_result == "success")
  {
    return makeSuccess("Describe completed for argument '" + argument + "'");
  }

  if (normalized_result == "failure")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Describe failed for argument '" + argument + "'");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Describe returned unsupported worker_result='" + worker_result + "'");
}
