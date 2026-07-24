#pragma once

#include <array>
#include <cstddef>

namespace filter {

/// Running average over the most recent N samples (default: 10).
class MovingAverage {
public:
    static constexpr std::size_t kWindowSize = 10;

    /// Insert a sample and return the average of the last up to 10 values.
    [[nodiscard]] double update(double sample) {
        if (count_ == kWindowSize) {
            sum_ -= samples_[index_];
        } else {
            ++count_;
        }

        samples_[index_] = sample;
        sum_ += sample;
        index_ = (index_ + 1) % kWindowSize;

        return sum_ / static_cast<double>(count_);
    }

    /// Current average, or 0.0 if no samples have been added.
    [[nodiscard]] double value() const {
        if (count_ == 0) {
            return 0.0;
        }
        return sum_ / static_cast<double>(count_);
    }

    [[nodiscard]] std::size_t size() const { return count_; }

    void reset() {
        samples_.fill(0.0);
        index_ = 0;
        count_ = 0;
        sum_ = 0.0;
    }

private:
    std::array<double, kWindowSize> samples_{};
    std::size_t index_{0};
    std::size_t count_{0};
    double sum_{0.0};
};

}  // namespace filter
