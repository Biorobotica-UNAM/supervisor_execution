#include "supervisor_execution/Approach.hpp"
#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Approach::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Approach requires a non-empty action argument");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("Approach timed out for argument '" + argument + "'");
  }

  const auto starts_with = [](const std::string & value, const std::string & prefix) {
    return value.rfind(prefix, 0U) == 0U;
  };

  const auto ends_with = [](const std::string & value, const std::string & suffix) {
    return value.size() >= suffix.size() &&
      value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
  };

  // Condiciones de fallo emitidas por tu FSM de Approach
  if (normalized_result == "failure" ||
      normalized_result == "goal_handle_inactive" ||
      normalized_result == "goal_cancelled" ||
      normalized_result == "target_not_in_knowledge" ||
      starts_with(normalized_result, "all_approach_angles_blocked:") ||
      starts_with(normalized_result, "all_angles_blocked:"))
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Approach failed for argument '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  // Condiciones de éxito emitidas por tu FSM de Approach
  if (normalized_result == "success" ||
      starts_with(normalized_result, "approached_safely:"))
  {
    return makeSuccess("Approach completed for argument '" + argument + "'");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Approach returned unsupported worker_result='" + worker_result + "'",
    supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}