#include "supervisor_execution/GoTo.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult GoTo::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "GoTo requires a non-empty destination argument");
  }

  if (normalized_result.find("bumper") != std::string::npos) {
    return makeFailure("bumper_triggered", worker_result,
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }
  if (normalized_result.find("location") != std::string::npos &&
      normalized_result.find("not found") != std::string::npos) {
    return makeFailure("location_not_found", worker_result,
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  if (normalized_result == "success")
  {
    return makeSuccess("GoTo reached destination '" + argument + "'");
  }

  // executor_tools::GoTo devuelve detalles como "arrived_at: cocina: ...";
  // el validador debe aceptarlos como exito, no como resultado mal formado.
  if (supervisor_execution::string_utils::startsWith(normalized_result, "arrived_at:"))
  {
    return makeSuccess("GoTo reached destination '" + argument + "'");
  }

  if (normalized_result == "timeout" ||
      supervisor_execution::string_utils::startsWith(normalized_result, "navigation_timeout"))
  {
    return makeTimeout("GoTo timed out while moving to '" + argument + "'");
  }

  if (normalized_result.find("timeout") != std::string::npos)
  {
    return makeTimeout("GoTo timed out while moving to '" + argument + "'");
  }

  if (normalized_result == "failure")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_NAVIGATION_FAILED,
      "GoTo failed while moving to '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  // Los errores detallados de navegacion tambien llegan con prefijo del worker.
  if (supervisor_execution::string_utils::startsWith(normalized_result, "navigation_failed:"))
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_NAVIGATION_FAILED,
      "GoTo failed while moving to '" + argument + "': " + worker_result,
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "GoTo returned unsupported worker_result='" + worker_result + "'",
    supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}
