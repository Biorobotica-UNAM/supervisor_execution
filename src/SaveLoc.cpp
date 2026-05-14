#include "supervisor_execution/SaveLoc.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult SaveLoc::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "SaveLoc requires a non-empty action argument");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("Find timed out for argument '" + argument + "'");
  }

  if (normalized_result == "success" || normalized_result == "saved")
  {
    return makeSuccess("Find completed for argument '" + argument + "'");
  }

  if (normalized_result == "failure" || normalized_result == "not_saved")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Find failed for argument '" + argument + "'");
  }

  if (normalized_result.rfind("saved:", 0) == 0)
  {
      return makeSuccess("SaveLoc completed for argument '" + argument + "'");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "SaveLoc returned unsupported worker_result='" + worker_result + "'");
}
