#include <gtest/gtest.h>
#include "supervisor_execution/GoTo.hpp"
#include "supervisor_execution/Find.hpp"
#include "supervisor_execution/AdditionalActions.hpp"

TEST(RecoveryContracts, LocationMissingAndBumperHaveStableReasons) {
  GoTo validator;
  EXPECT_EQ(validator.validate("missing", "navigation_failed: missing: location not found").failure_reason,
    "location_not_found");
  EXPECT_EQ(validator.validate("kitchen", "navigation_failed: bumper triggered").failure_reason,
    "bumper_triggered");
  EXPECT_TRUE(validator.validate("kitchen", "arrived_at: kitchen: ok").approved);
  EXPECT_FALSE(validator.validate("kitchen", "nonsense").approved);
}
TEST(RecoveryContracts, DefaultFailureHasFailureStatus) {
  const auto result = makeFailure("invalid_argument", "bad argument");
  EXPECT_FALSE(result.approved);
  EXPECT_EQ(result.status, supervisor_interfaces::msg::TaskExecutionReport::FAILURE);
}
TEST(RecoveryContracts, ObjectFailureIsNotSuccess) {
  Find validator;
  EXPECT_FALSE(validator.validate("cup_1", "cup_not_found").approved);
  EXPECT_FALSE(validator.validate("cup_1", "object_not_found:cup_1").approved);
  EXPECT_TRUE(validator.validate("cup_1", "cup_found").approved);
}
TEST(RecoveryContracts, AdditionalActionsAndPartialPhysicalEffects) {
  EXPECT_TRUE(validateAdditionalAction("give", "cup_1", "cup_1_delivered_successfully").approved);
  EXPECT_TRUE(validateAdditionalAction("receive", "cup_1", "cup_1_received_successfully").approved);
  EXPECT_TRUE(validateAdditionalAction("find_space", "table", "find_space_tf_ready frame=table").approved);
  EXPECT_TRUE(validateAdditionalAction("analyze_person", "{}", "person_analyzed:person_found").approved);
  EXPECT_FALSE(validateAdditionalAction("give", "cup_1", "other_delivered_successfully").approved);
  EXPECT_EQ(validateAdditionalAction("give", "cup_1", "arm_reset_failed_after_give").failure_reason,
    "physical_effect_uncertain");
  EXPECT_EQ(validateAdditionalAction("receive", "cup_1", "report_object_failed").failure_reason,
    "physical_effect_uncertain");
  EXPECT_FALSE(validateAdditionalAction("find_space", "table", "find_space_tf_not_available").approved);
  EXPECT_FALSE(validateAdditionalAction("analyze_person", "{}", "person_pose_failed_bbox_to_pose").approved);
}
