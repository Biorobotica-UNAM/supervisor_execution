#include "supervisor_execution/Fold.hpp"

#include "supervisor_execution/StringUtils.hpp"

ActionValidationResult Fold::validate(
  const std::string & argument,
  const std::string & worker_result) const
{
  const auto normalized_argument = supervisor_execution::string_utils::normalize(argument);
  const auto normalized_result = supervisor_execution::string_utils::normalize(worker_result);
  
  const auto matches_result_token = [&normalized_result](const std::string & token) {
    return normalized_result == token ||
           supervisor_execution::string_utils::startsWith(normalized_result, token + ";");
  };

  // ── VALIDADOR 1: CONTROL DE ARGUMENTOS VACÍOS ────────────────────────
  if (normalized_argument.empty())
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_INVALID_ARGUMENT,
      "Fold Action Error: Requires a target item name (e.g., 'shirt', 'cloth')");
  }

  // ── VALIDADOR 2: CASOS DE ÉXITO EXITOSO (SUCCESS) ────────────────────
  // Mapeamos los strings exactos que devuelve el retorno final de tu Fold::run
  if (matches_result_token("success") ||
      matches_result_token("cloth_folded") ||
      matches_result_token("cloth_folded_successfully") ||
      normalized_result.find("folded_successfully") != std::string::npos)
  {
    return makeSuccess("Fold Action Success: Characterized and folded '" + argument + "' successfully.");
  }

  // ── VALIDADOR 3: CASOS DE TIEMPO AGOTADO (TIMEOUT) ───────────────────
  if (normalized_result == "timeout" ||
      normalized_result.find("timeout") != std::string::npos)
  {
    return makeTimeout("Fold Action Timeout: Operation exceeded maximum allowed time on '" + argument + "'");
  }

  // ── VALIDADOR 4: ENRUTAMIENTO DE FALLAS ESPECÍFICAS (FAILURES) ───────
  
  // A) Si falló la fase 0: Percepción (Detección de la playera)
  if (normalized_result == "cloth_not_detected" || 
      normalized_result.find("not_detected") != std::string::npos)
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_UNSPECIFIED,
      "Fold Action Failed [Perception]: YOLO/RGBD could not find keypoints for '" + argument + "'",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  // B) Si la meta fue cancelada por la FSM de CLIPS a mitad de camino
  if (normalized_result == "goal_cancelled" || normalized_result == "base_advance_cancelled")
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_UNSPECIFIED,
      "Fold Action Failed [Interrupted]: Goal was cancelled during execution.",
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  // C) Si fallaron las fases de manipulación (Cualquier paso de brazos, grippers o trayectorias)
  // Atrapa: sleeve_approach_arm_failed, sleeve_grip_failed, left_sleeve_fold_failed, bottom_grip_failed, etc.
  if (normalized_result.find("failed") != std::string::npos || 
      normalized_result.find("arm") != std::string::npos ||
      normalized_result.find("grip") != std::string::npos)
  {
    return makeFailure(
      supervisor_interfaces::msg::TaskExecutionReport::REASON_UNSPECIFIED,
      "Fold Action Failed [Manipulation]: Kinematics or Gripper error. Detail: " + worker_result,
      supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
  }

  // D) Fallo genérico de seguridad por si llega un string malformado del worker
  return makeFailure(
    supervisor_interfaces::msg::TaskExecutionReport::REASON_UNSPECIFIED,
    "Fold Action Failed [Unknown]: Malformed or unsupported worker report: '" + worker_result + "'",
    supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}