#pragma once

#include <string>

#include "supervisor_execution/ActionValidationResult.hpp"

class Analyze
{
public:
  ActionValidationResult validate(
    const std::string & argument,
    const std::string & worker_result) const;
};