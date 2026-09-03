//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.h
//
// Identification: src/include/buffer/clock_replacer.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <mutex>  // NOLINT
#include <optional>
#include <vector>

#include "buffer/simple_replacer.h"
#include "common/config.h"
#include "common/macros.h"

namespace bustub {

/**
 * ClockReplacer implements the CLOCK (a.k.a. second-chance) replacement policy,
 * which approximates LRU with a single reference bit per frame and a rotating
 * "hand". It is a `SimpleReplacer`, so a `BufferPoolManager` can use it in place
 * of `LRUKReplacer` (request it by name through `MakeReplacer`; see
 * "buffer/replacer_factory.h").
 *
 * This replacer is not part of the graded Project 1 assignment; it is provided as
 * an alternative eviction policy.
 */
class ClockReplacer : public SimpleReplacer {
 public:
  using SimpleReplacer::RecordAccess;  // unhide overloads

  explicit ClockReplacer(size_t num_frames);

  DISALLOW_COPY_AND_MOVE(ClockReplacer);

  ~ClockReplacer() override = default;

  auto Evict() -> std::optional<frame_id_t> override;
  void RecordAccess(frame_id_t frame_id, page_id_t page_id, AccessType access_type) override;
  void SetEvictable(frame_id_t frame_id, bool set_evictable) override;
  void Remove(frame_id_t frame_id) override;
  auto Size() -> size_t override;

 private:
  // TODO(student): implement me! You can replace or remove these member variables as you like.
  /** Whether the frame is currently tracked by the replacer. */
  std::vector<bool> present_;
  /** Whether the frame may be selected for eviction (counts toward `Size`). */
  std::vector<bool> evictable_;
  /** The CLOCK reference ("second chance") bit for each frame. */
  std::vector<bool> ref_;
  /** Number of frames the replacer can track; frame ids are in [0, num_frames_). */
  [[maybe_unused]] size_t num_frames_;
  /** Number of evictable frames. */
  [[maybe_unused]] size_t curr_size_{0};
  /** The rotating clock hand. */
  [[maybe_unused]] frame_id_t hand_{0};
  std::mutex latch_;
};

}  // namespace bustub
