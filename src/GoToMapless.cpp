#include "supervisor_execution/GoToMapless.hpp"
#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult GoToMapless::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result   = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "GoToMapless requires a non-empty destination argument");
  }

  // Éxito explícito
  if (normalized_result == "success" ||
      supervisor_execution::string_utils::startsWith(normalized_result, "arrived_mapless:"))
  {
    return makeSuccess("GoToMapless reached '" + argument + "'");
  }

  // Timeout o servicio no disponible
  if (normalized_result == "timeout" ||
      supervisor_execution::string_utils::startsWith(normalized_result, "mapless_navigation_timeout:"))
  {
    return makeTimeout("GoToMapless timed out while moving to '" + argument + "'");
  }

  // Fallos del servicio
  if (supervisor_execution::string_utils::startsWith(normalized_result, "mapless_goto_service_unavailable") ||
      supervisor_execution::string_utils::startsWith(normalized_result, "mapless_goto_service_timeout"))
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_NAVIGATION_FAILED,
      "GoToMapless service call failed for '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  if (supervisor_execution::string_utils::startsWith(normalized_result, "mapless_goto_rejected:"))
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "GoToMapless rejected for '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  // Cualquier otro resultado se considera mal formado
  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "GoToMapless returned unsupported worker_result='" + worker_result + "'");
}