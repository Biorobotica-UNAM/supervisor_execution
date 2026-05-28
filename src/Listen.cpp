#include "supervisor_execution/Listen.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Listen::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  (void)argument;  // ya no se usa, pero se deja para mantener la interfaz

  const auto normalized_result =
    supervisor_execution::string_utils::normalize(worker_result);

  // Casos sin entrada o fallos reales del worker Listen.
  if (normalized_result.empty() ||
      normalized_result == "no_input" ||
      normalized_result == "nodata" ||
      normalized_result == "no_data" ||
      normalized_result == "listen_failed")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_NO_INPUT,
      "Listen returned no usable speech",
      supervisor_interfaces::msg::TaskExecutionReport::REJECTED);
  }

  if (supervisor_execution::string_utils::startsWith(normalized_result, "invalid_argument:") ||
      supervisor_execution::string_utils::startsWith(normalized_result, "entity_not_found:") ||
      normalized_result == "update_world_state_failed")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
      "Listen failed with worker_result='" + worker_result + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  // Caso válido: cualquier cosa reconocida
  return makeSuccess("Listen accepted result '" + worker_result + "'");
}
