#include "supervisor_execution/SaveLoc.hpp"
#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult SaveLoc::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result   = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "SaveLoc requires a non-empty action argument");
  }

  // Éxito: el worker devuelve "location_saved: <nombre>"
  if (normalized_result == "success" ||
      supervisor_execution::string_utils::startsWith(normalized_result, "location_saved:"))
  {
    return makeSuccess("SaveLoc saved '" + argument + "'");
  }

  // Fallo controlado
  if (normalized_result == "failure" ||
      supervisor_execution::string_utils::startsWith(normalized_result, "save_failed:"))
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_NAVIGATION_FAILED,
      "SaveLoc failed for '" + argument + "'");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("SaveLoc timed out for '" + argument + "'");
  }

  // Cualquier otro resultado se considera mal formado
  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "SaveLoc returned unsupported worker_result='" + worker_result + "'");
}