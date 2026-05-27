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

  // executor_tools::Focus reporta "focus_updated" cuando /vision/focus acepta
  // el cambio; para el supervisor eso equivale a exito.
  if (normalized_result == "success" || normalized_result == "focus_updated")
  {
    return makeSuccess("Focus completed for argument '" + argument + "'");
  }

  // "focus_failed" es el fallo real del worker, distinto de malformed_result.
  if (normalized_result == "failure" || normalized_result == "focus_failed")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Focus failed for argument '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }
  
  if (normalized_result.rfind("focus:", 0) == 0)
  {
      return makeSuccess("Focus completed for argument '" + argument + "'");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Focus returned unsupported worker_result='" + worker_result + "'",
    supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}
