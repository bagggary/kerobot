#pragma once
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

namespace kerobot {
enum class ErrorCode { invalid_units, invalid_geometry, unconfirmed_joint, invalid_wps, invalid_id, conflict, io, hardware_disabled };
struct Error : std::runtime_error {
  ErrorCode code;
  Error(ErrorCode c, const std::string& message) : std::runtime_error(message), code(c) {}
};
struct Vec3 { double x, y, z; };
struct JointIntent {
  std::string revision;
  Vec3 start_m, end_m;
  int side; // +1 or -1; synthetic fixture normal lies along Y
  bool confirmed;
};
struct TestWps {
  std::string revision;
  std::string status; // only test-only is supported
  double travel_speed_m_s;
  double wire_feed_speed_m_s;
};
struct Plan {
  std::string joint_revision, wps_revision;
  Vec3 tangent, side_normal;
  double travel_speed_m_s, wire_feed_speed_m_s;
  std::vector<Vec3> points_m;
};
double to_metres(double value, const std::string& unit);
Plan nominal_line(const JointIntent&, const TestWps&, unsigned intervals);
std::string draft_json(const std::string& job_id, const Plan&);
// Immutable per-JOBID draft directories. Exact replay is idempotent; changes conflict.
void persist_draft(const std::filesystem::path& root, const std::string& job_id, const std::string& payload);
class MockController {
public:
  bool supports_motion() const { return false; }
  [[noreturn]] void start() const;
};
} // namespace kerobot
