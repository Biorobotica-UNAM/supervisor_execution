#include "supervisor_execution/Say.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Say::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Say requires a non-empty action argument");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("Say timed out for argument '" + argument + "'");
  }

  if (normalized_result == "success")
  {
    return makeSuccess("Say completed for argument '" + argument + "'");
  }

  if (supervisor_execution::string_utils::startsWith(normalized_result, "uttered:"))
  {
    return makeSuccess("Say uttered phrase for argument '" + argument + "'");
  }

  if (normalized_result == "tts_failed" || normalized_result == "failure")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_TTS_FAILED,
      "Say failed because TTS did not complete");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Say returned unsupported worker_result='" + worker_result + "'");
}
