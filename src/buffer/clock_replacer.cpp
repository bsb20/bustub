//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.cpp
//
// Identification: src/buffer/clock_replacer.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/clock_replacer.h"

namespace bustub {

/**
 * @brief a new ClockReplacer.
 * @param num_frames the maximum number of frames the ClockReplacer will be required to store
 */
ClockReplacer::ClockReplacer(size_t num_frames)
    : present_(num_frames, false), evictable_(num_frames, false), ref_(num_frames, false), num_frames_(num_frames) {}

/**
 * @brief Find a frame to evict using the CLOCK (second-chance) policy.
 * @return the id of the evicted frame, or std::nullopt if no frame can be evicted.
 */
auto ClockReplacer::Evict() -> std::optional<frame_id_t> { return std::nullopt; }

/**
 * @brief Record an access to `frame_id`, setting its reference bit.
 *
 * @param frame_id id of frame that received a new access.
 * @param page_id id of the page held by the frame. Available to every replacer, not needed by CLOCK.
 * @param access_type type of access. Not needed by CLOCK.
 */
void ClockReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] page_id_t page_id,
                                 [[maybe_unused]] AccessType access_type) {}

/**
 * @brief Toggle whether a tracked frame is a candidate for eviction.
 */
void ClockReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {}

/**
 * @brief Stop tracking `frame_id` entirely, regardless of its reference bit.
 */
void ClockReplacer::Remove(frame_id_t frame_id) {}

/**
 * @brief Return the number of evictable frames.
 */
auto ClockReplacer::Size() -> size_t { return 0; }

}  // namespace bustub
