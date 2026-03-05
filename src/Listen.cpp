#include "supervisor_execution/Listen.hpp"

#include <algorithm>

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Listen::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto expected_tokens = supervisor_execution::string_utils::splitTokens(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);

  if (expected_tokens.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Listen requires at least one expected token in argument");
  }

  if (normalized_result == "timeout")
  {
    return makeTimeout("Listen timed out while waiting for '" + argument + "'");
  }

  if (normalized_result.empty() || normalized_result == "no_input")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_NO_INPUT,
      "Listen did not detect any input");
  }

  const bool wildcard_requested = std::any_of(
    expected_tokens.begin(),
    expected_tokens.end(),
    [](const std::string & token) {return token == "any" || token == "*";});

  if (wildcard_requested)
  {
    return makeSuccess("Listen accepted recognized token '" + worker_result + "'");
  }

  const bool exact_match = std::find(
    expected_tokens.begin(),
    expected_tokens.end(),
    normalized_result) != expected_tokens.end();

  if (exact_match)
  {
    return makeSuccess(
      "Listen matched expected token '" + worker_result + "'");
  }

  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_ARGUMENT_MISMATCH,
    "Listen expected '" + argument + "' but received '" + worker_result + "'");
}
