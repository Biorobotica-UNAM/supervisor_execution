#include "supervisor_execution/Listen.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Listen::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  (void)argument;  // ya no se usa, pero se deja para mantener la interfaz

  const auto normalized_result =
    supervisor_execution::string_utils::normalize(worker_result);

  // Caso: timeout del recognizer
  if (normalized_result == "timeout")
  {
    return makeTimeout("Listen timed out while waiting for input");
  }

  // Caso: no se detectó nada
  if (normalized_result.empty() ||
      normalized_result == "no_input" ||
      normalized_result == "nodata" ||
      normalized_result == "no_data")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_NO_INPUT,
      "Listen returned NoData");
  }

  // Caso válido: cualquier cosa reconocida
  return makeSuccess("Listen accepted result '" + worker_result + "'");
}