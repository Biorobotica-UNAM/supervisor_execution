#include "supervisor_execution/Fold.hpp"
#include "supervisor_execution/StringUtils.hpp"
#include <supervisor_interfaces/msg/task_execution_report.hpp>

ActionValidationResult Fold::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto result =
    supervisor_execution::string_utils::normalize(worker_result);

  if (result == "success" ||
      result == "cloth_folded_successfully")
  {
    return makeSuccess("Fold completed for '" + argument + "'");
  }

  if (result.find("goal_cancelled") != std::string::npos)
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
      "Fold cancelled for '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  if (result.find("timeout") != std::string::npos ||
      result.find("cloth_not_detected") != std::string::npos)
  {
    return makeTimeout("Fold timed out for '" + argument + "'");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_MALFORMED_RESULT,
    "Fold returned unsupported result='" + worker_result + "'",
    supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}
