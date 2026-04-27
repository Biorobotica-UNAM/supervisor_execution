#include "supervisor_execution/MemorizeFace.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult MemorizeFace::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "MemorizeFace requires a non-empty action argument");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("MemorizeFace timed out for argument '" + argument + "'");
  }

  if (normalized_result == "success")
  {
    return makeSuccess("MemorizeFace completed for argument '" + argument + "'");
  }

  if (normalized_result == "failure")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "MemorizeFace failed for argument '" + argument + "'");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "MemorizeFace returned unsupported worker_result='" + worker_result + "'");
}
