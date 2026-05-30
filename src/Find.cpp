#include "supervisor_execution/Find.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Find::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Find requires a non-empty action argument");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("Find timed out for argument '" + argument + "'");
  }

  const auto starts_with = [](const std::string & value, const std::string & prefix) {
    return value.rfind(prefix, 0U) == 0U;
  };

  const auto ends_with = [](const std::string & value, const std::string & suffix) {
    return value.size() >= suffix.size() &&
      value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
  };

  if (normalized_result == "failure" ||
      normalized_result == "not_found" ||
      starts_with(normalized_result, "person_not_found:") ||
      ends_with(normalized_result, "_not_found"))
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Find failed for argument '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  if (normalized_result == "success" ||
      normalized_result == "found" ||
      starts_with(normalized_result, "found:") ||
      starts_with(normalized_result, "person_found:") ||
      ends_with(normalized_result, "_found"))
  {
    return makeSuccess("Find completed for argument '" + argument + "'");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Find returned unsupported worker_result='" + worker_result + "'",
    supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}
