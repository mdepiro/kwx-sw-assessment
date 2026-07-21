#pragma once

#include <cmath>
#include <concepts>
#include <random>

namespace pose {

struct Vector3 {
    double x{0.0};
    double y{0.0};
    double z{0.0};
}; //meters

struct Quaternion {
    double w{1.0};
    double x{0.0};
    double y{0.0};
    double z{0.0};


    void normalize() {
        double norm = std::sqrt(w * w + x * x + y * y + z * z);
        if (norm > 0.0) {
            w /= norm;
            x /= norm;
            y /= norm;
            z /= norm;
        } else {
            w = 1.0;
            x = y = z = 0.0;
        }
    }
};

///6-DoF Pose (Position + Quaternion).
struct Pose {
    Vector3 position{};
    Quaternion orientation{};
};

/// Noise Error meters and radians
struct PoseNoiseConfig {
    double pos_std_x{0.002};
    double pos_std_y{0.002};
    double pos_std_z{0.005};
    double quat_std_w{0.01};
    double quat_std_x{0.01};
    double quat_std_y{0.01};
    double quat_std_z{0.01};
};

/// 
class GetPose {
public:
    /// @brief Construct with an optional random seed (constrained via C++20 concept).
    template <typename SeedType>
        requires std::convertible_to<SeedType, unsigned int>
    explicit GetPose(SeedType seed)
        : rng_(static_cast<unsigned int>(seed)) {}

    GetPose()
        : rng_(std::random_device{}()) {}

    ///Generate noisy values
    [[nodiscard]] Pose generate(const Pose& input_pose,
                                const PoseNoiseConfig& noise = PoseNoiseConfig{}) {
        std::normal_distribution<double> dist_px(0.0, noise.pos_std_x);
        std::normal_distribution<double> dist_py(0.0, noise.pos_std_y);
        std::normal_distribution<double> dist_pz(0.0, noise.pos_std_z);

        std::normal_distribution<double> dist_qw(0.0, noise.quat_std_w);
        std::normal_distribution<double> dist_qx(0.0, noise.quat_std_x);
        std::normal_distribution<double> dist_qy(0.0, noise.quat_std_y);
        std::normal_distribution<double> dist_qz(0.0, noise.quat_std_z);

        Pose output = input_pose;

        // meters
        output.position.x += dist_px(rng_);
        output.position.y += dist_py(rng_);
        output.position.z += dist_pz(rng_);

        // radians
        output.orientation.w += dist_qw(rng_);
        output.orientation.x += dist_qx(rng_);
        output.orientation.y += dist_qy(rng_);
        output.orientation.z += dist_qz(rng_);

        // Re-normalize quaternion
        output.orientation.normalize();

        return output;
    }

private:
    std::mt19937_64 rng_;
};

}  // namespace pose