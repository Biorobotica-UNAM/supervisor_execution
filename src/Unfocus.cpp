#include "supervisor_execution/Unfocus.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Unfocus::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_result == "timeout")
  {
    return makeTimeout("Unfocus timed out for argument '" + argument + "'");
  }

  if (normalized_result == "success")
  {
    return makeSuccess("Unfocus completed for argument '" + argument + "'");
  }

  if (normalized_result == "failure")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Unfocus failed for argument '" + argument + "'");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Unfocus returned unsupported worker_result='" + worker_result + "'");
}
