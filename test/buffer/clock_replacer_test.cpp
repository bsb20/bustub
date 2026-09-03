//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer_test.cpp
//
// Identification: test/buffer/clock_replacer_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstdio>
#include <optional>
#include <thread>  // NOLINT
#include <vector>

#include "buffer/clock_replacer.h"
#include "gtest/gtest.h"

namespace bustub {

TEST(ClockReplacerTest, DISABLED_SampleTest) {
  ClockReplacer clock_replacer(7);

  // Scenario: add six frames to the replacer as evictable. Recording an access sets each frame's
  // reference ("second chance") bit; marking it evictable makes it a candidate for eviction.
  for (frame_id_t f = 1; f <= 6; f++) {
    clock_replacer.RecordAccess(f);
    clock_replacer.SetEvictable(f, true);
  }
  clock_replacer.RecordAccess(1);  // a second access to frame 1 (its reference bit is already set)
  EXPECT_EQ(6, clock_replacer.Size());

  // Scenario: get three victims from the clock.
  EXPECT_EQ(1, clock_replacer.Evict());
  EXPECT_EQ(2, clock_replacer.Evict());
  EXPECT_EQ(3, clock_replacer.Evict());

  // Scenario: pin frames in the replacer (mark them non-evictable).
  // Note that 3 has already been evicted, so pinning 3 should have no effect.
  clock_replacer.SetEvictable(3, false);
  clock_replacer.SetEvictable(4, false);
  EXPECT_EQ(2, clock_replacer.Size());

  // Scenario: unpin 4. We expect that the reference bit of 4 will be set.
  clock_replacer.RecordAccess(4);
  clock_replacer.SetEvictable(4, true);

  // Scenario: continue looking for victims. We expect these victims.
  EXPECT_EQ(5, clock_replacer.Evict());
  EXPECT_EQ(6, clock_replacer.Evict());
  EXPECT_EQ(4, clock_replacer.Evict());
}

}  // namespace bustub
