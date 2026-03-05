#pragma once

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace supervisor_execution
{
namespace string_utils
{
inline std::string trim(const std::string & value)
{
  const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char c) {
    return std::isspace(c) != 0;
  });
  if (first == value.end()) {
    return "";
  }

  const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char c) {
    return std::isspace(c) != 0;
  }).base();

  return std::string(first, last);
}

inline std::string toLower(std::string value)
{
  std::transform(
    value.begin(),
    value.end(),
    value.begin(),
    [](unsigned char c) {return static_cast<char>(std::tolower(c));});
  return value;
}

inline std::string normalize(const std::string & value)
{
  return toLower(trim(value));
}

inline bool startsWith(const std::string & value, const std::string & prefix)
{
  return value.rfind(prefix, 0U) == 0U;
}

inline std::vector<std::string> splitTokens(const std::string & value)
{
  std::string normalized = value;
  std::replace(normalized.begin(), normalized.end(), ',', '|');

  std::stringstream ss(normalized);
  std::string token;
  std::vector<std::string> output;

  while (std::getline(ss, token, '|'))
  {
    auto cleaned = normalize(token);
    if (!cleaned.empty()) {
      output.push_back(cleaned);
    }
  }

  return output;
}
}  // namespace string_utils
}  // namespace supervisor_execution
