#pragma once

#include <string>

#include "supervisor_execution/ActionValidationResult.hpp"

class VerifyTake
{
public:
  ActionValidationResult validate(
    const std::string & argument,
    const std::string & worker_result) const;
};
