#include "supervisor_execution/Analyze.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Analyze::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Analyze requires a non-empty action argument");
  }

  // Control de Timeouts
  if (normalized_result == "timeout")
  {
    return makeTimeout("Analyze timed out for argument '" + argument + "'");
  }

  // Lambda helpers idénticos a los de Find para analizar prefijos/sufijos
  const auto starts_with = [](const std::string & value, const std::string & prefix) {
    return value.rfind(prefix, 0U) == 0U;
  };

  const auto ends_with = [](const std::string & value, const std::string & suffix) {
    return value.size() >= suffix.size() &&
      value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
  };

  // valuar casos de Error/Fallo que definimos en el Worker Node
  if (normalized_result == "failure" ||
      normalized_result == "invalid_json_argument" ||
      normalized_result == "missing_action_field" ||
      normalized_result == "unknown_action" ||
      normalized_result == "analytics_failed" ||
      normalized_result == "service_failed_or_no_objects" ||
      ends_with(normalized_result, "failed"))
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Analyze failed for argument '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  
  if (normalized_result == "success" ||
      starts_with(normalized_result, "counted_") ||
      starts_with(normalized_result, "size_filter_") ||
      starts_with(normalized_result, "color_match_") ||
      starts_with(normalized_result, "compare_winner_") ||
      ends_with(normalized_result, "success"))
  {
    return makeSuccess("Analyze completed for argument '" + argument + "'");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Analyze returned unsupported worker_result='" + worker_result + "'",
    supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}