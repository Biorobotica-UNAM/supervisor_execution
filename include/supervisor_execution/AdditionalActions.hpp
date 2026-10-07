#pragma once
#include "supervisor_execution/ActionValidationResult.hpp"
#include "supervisor_execution/StringUtils.hpp"

// Explicit success contracts. Unknown results never count as successful.
inline ActionValidationResult validateAdditionalAction(
  const std::string & action, const std::string & argument, const std::string & worker_result)
{
  using supervisor_execution::string_utils::normalize;
  using supervisor_execution::string_utils::startsWith;
  const auto result = normalize(worker_result);
  if (argument.empty()) {return makeFailure("invalid_argument", "Empty argument",
    supervisor_interfaces::msg::TaskExecutionReport::REJECTED);}
  if (result == "timeout") {return makeTimeout(worker_result);}
  if ((action == "give" && result == normalize(argument) + "_delivered_successfully") ||
      (action == "receive" && result == normalize(argument) + "_received_successfully") ||
      (action == "find_space" && startsWith(result, "find_space_tf_ready ")) ||
      (action == "analyze_person" && startsWith(result, "person_analyzed:"))) {
    return makeSuccess(worker_result);
  }
  // Partial physical effects are uncertain: stop rather than replay the action.
  if ((action == "give" || action == "receive") &&
      (result == "report_object_failed" || result == "gripper_close_failed" ||
       startsWith(result, "arm_reset_failed_after_"))) {
    return makeFailure("physical_effect_uncertain", worker_result,
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }
  return makeFailure(action + "_failed", worker_result,
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}
