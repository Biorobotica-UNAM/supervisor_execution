#include "supervisor_execution/Give.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Give::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument =
    supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result =
    supervisor_execution::string_utils::normalize(worker_result);

  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Give requires a non-empty target argument");
  }

  const auto starts_with = [](const std::string & value, const std::string & prefix) {
    return value.rfind(prefix, 0U) == 0U;
  };

  const auto ends_with = [](const std::string & value, const std::string & suffix) {
    return value.size() >= suffix.size() &&
      value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
  };

  if (normalized_result == "timeout" ||
      normalized_result.find("timeout") != std::string::npos)
  {
    return makeTimeout("Give timed out for argument '" + argument + "'");
  }

  if (normalized_result == "success" ||
      normalized_result == "give_completed" ||
      starts_with(normalized_result, "give_completed_for_") ||
      ends_with(normalized_result, "_delivered_successfully"))
  {
    return makeSuccess("Give completed for argument '" + argument + "'");
  }

  if (normalized_result == "not_object" ||
      normalized_result == "failure" ||
      normalized_result == "goal_handle_inactive" ||
      normalized_result == "arm_extension_failed" ||
      normalized_result == "gripper_open_failed" ||
      normalized_result == "report_object_failed" ||
      normalized_result == "gripper_close_failed" ||
      normalized_result == "arm_reset_failed_after_give" ||
      ends_with(normalized_result, "failed"))
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Give failed for argument '" + argument + "' (" + worker_result + ")",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Give returned unsupported worker_result='" + worker_result + "'",
    supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}

