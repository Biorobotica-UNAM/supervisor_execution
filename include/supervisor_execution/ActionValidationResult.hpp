#pragma once

#include <cstdint>
#include <string>

#include "supervisor_interfaces/msg/task_execution_report.hpp"

struct ActionValidationResult
{
  bool approved{false};
  uint8_t status{supervisor_interfaces::msg::TaskExecutionReport::FAILURE};
  std::string failure_reason{
    supervisor_interfaces::msg::TaskExecutionReport::REASON_UNSPECIFIED};
  std::string details;
};

inline ActionValidationResult makeSuccess(const std::string & details)
{
  ActionValidationResult result;
  result.approved = true;
  result.status = supervisor_interfaces::msg::TaskExecutionReport::SUCCESS;
  result.failure_reason = supervisor_interfaces::msg::TaskExecutionReport::REASON_NONE;
  result.details = details;
  return result;
}

inline ActionValidationResult makeFailure(
  const std::string & reason,
  const std::string & details,
  uint8_t status = supervisor_interfaces::msg::TaskExecutionReport::SUCCESS)
{
  ActionValidationResult result;
  result.approved = true;
  result.status = status;
  result.failure_reason = reason;
  result.details = details;
  return result;
}

inline ActionValidationResult makeTimeout(const std::string & details)
{
  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_TIMEOUT,
    details,
    supervisor_interfaces::msg::TaskExecutionReport::TIMEOUT);
}
