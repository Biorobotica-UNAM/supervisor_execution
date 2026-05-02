#include "supervisor_execution/Focus.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Focus::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Focus requires a non-empty action argument");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("Focus timed out for argument '" + argument + "'");
  }

  if (normalized_result == "success")
  {
    return makeSuccess("Focus completed for argument '" + argument + "'");
  }

  if (normalized_result == "failure")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Focus failed for argument '" + argument + "'");
  }
  
  if (normalized_result.rfind("focus:", 0) == 0)
  {
      return makeSuccess("Focus completed for argument '" + argument + "'");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Focus returned unsupported worker_result='" + worker_result + "'");
}
