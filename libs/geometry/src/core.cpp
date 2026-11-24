#include "kerobot/core.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <locale>
#include <sstream>

namespace kerobot {
namespace {
bool finite(Vec3 v) { return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z); }
void identifier(const std::string& id) {
  if (id.empty() || id.size() > 80) throw Error(ErrorCode::invalid_id, "ID length must be 1..80");
  for (const unsigned char c : id)
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_'))
      throw Error(ErrorCode::invalid_id, "ID contains unsupported characters");
}
}
double to_metres(double value, const std::string& unit) {
  if (!std::isfinite(value)) throw Error(ErrorCode::invalid_units, "Non-finite length");
  double scale = 0;
  if (unit == "m") scale = 1;
  else if (unit == "mm") scale = 0.001;
  else throw Error(ErrorCode::invalid_units, "Explicit m or mm required");
  const double result = value * scale;
  if (!std::isfinite(result)) throw Error(ErrorCode::invalid_units, "Length overflow");
  return result;
}
Plan nominal_line(const JointIntent& j, const TestWps& w, unsigned intervals) {
  identifier(j.revision); identifier(w.revision);
  if (!j.confirmed) throw Error(ErrorCode::unconfirmed_joint, "Operator must confirm intent");
  if (!finite(j.start_m) || !finite(j.end_m) || (j.side != 1 && j.side != -1) || intervals < 1 || intervals > 10000)
    throw Error(ErrorCode::invalid_geometry, "Invalid coordinates, side or sample count");
  Vec3 d{j.end_m.x-j.start_m.x, j.end_m.y-j.start_m.y, j.end_m.z-j.start_m.z};
  const double length = std::hypot(d.x, d.y, d.z);
  // Sprint 01 analytic fixture supports only an X-aligned synthetic straight seam.
  if (!std::isfinite(length) || length < 1e-9 || std::abs(d.y) > 1e-12 || std::abs(d.z) > 1e-12)
    throw Error(ErrorCode::invalid_geometry, "Only nonzero X-aligned synthetic seams supported");
  if (w.status != "test-only" || !std::isfinite(w.travel_speed_m_s) || !std::isfinite(w.wire_feed_speed_m_s) ||
      w.travel_speed_m_s <= 0 || w.travel_speed_m_s > 0.02 || w.wire_feed_speed_m_s <= 0 || w.wire_feed_speed_m_s > 0.5)
    throw Error(ErrorCode::invalid_wps, "Invalid test WPS; synthetic limits are not qualified process limits");
  Plan p{j.revision, w.revision, {d.x/length,0,0}, {0,static_cast<double>(j.side),0}, w.travel_speed_m_s, w.wire_feed_speed_m_s, {}};
  for (unsigned i=0; i<=intervals; ++i) {
    const double t = static_cast<double>(i)/intervals;
    p.points_m.push_back({j.start_m.x+t*d.x,j.start_m.y,j.start_m.z});
  }
  return p;
}
std::string draft_json(const std::string& job_id, const Plan& p) {
  identifier(job_id); identifier(p.joint_revision); identifier(p.wps_revision);
  if (p.points_m.size()<2 || !finite(p.tangent) || !finite(p.side_normal)) throw Error(ErrorCode::invalid_geometry,"Invalid plan");
  if (!std::isfinite(p.travel_speed_m_s) || !std::isfinite(p.wire_feed_speed_m_s) ||
      p.travel_speed_m_s <= 0 || p.travel_speed_m_s > 0.02 || p.wire_feed_speed_m_s <= 0 || p.wire_feed_speed_m_s > 0.5)
    throw Error(ErrorCode::invalid_wps,"Invalid plan speeds");
  std::ostringstream s; s.imbue(std::locale::classic()); s << std::setprecision(17);
  s << "{\"schema_version\":1,\"job_id\":\"" << job_id
    << "\",\"state\":\"draft\",\"synthetic\":true,\"release_eligible\":false,\"units\":\"m\",\"joint_revision\":\""
    << p.joint_revision << "\",\"wps_revision\":\"" << p.wps_revision << "\",\"points_m\":[";
  bool first=true;
  for (auto v:p.points_m) {
    if (!finite(v)) throw Error(ErrorCode::invalid_geometry,"Non-finite plan");
    if (!first) s << ',';
    first=false; s << '[' << v.x << ',' << v.y << ',' << v.z << ']';
  }
  s << "],\"travel_speed_m_s\":" << p.travel_speed_m_s << ",\"wire_feed_speed_m_s\":" << p.wire_feed_speed_m_s << ",\"tangent\":[" << p.tangent.x << ',' << p.tangent.y << ',' << p.tangent.z
    << "],\"side_normal\":[" << p.side_normal.x << ',' << p.side_normal.y << ',' << p.side_normal.z << "]}\n";
  return s.str();
}
void persist_draft(const std::filesystem::path& root, const std::string& job_id, const std::string& payload) {
  identifier(job_id);
  std::error_code ec;
  std::filesystem::create_directories(root,ec);
  if (ec) throw Error(ErrorCode::io,"Cannot create draft root");
  const auto directory = root/job_id;
  const bool owner = std::filesystem::create_directory(directory,ec);
  if (ec) throw Error(ErrorCode::io,"Cannot reserve JOBID");
  const auto file = directory/"draft.json";
  if (!owner) {
    std::ifstream in(file,std::ios::binary);
    if (!in) throw Error(ErrorCode::conflict,"Existing draft is incomplete; manual reconciliation required");
    const std::string existing{std::istreambuf_iterator<char>(in),{}};
    if (existing != payload) throw Error(ErrorCode::conflict,"JOBID already binds another draft");
    return;
  }
  const auto temporary=directory/"draft.tmp";
  std::ofstream out(temporary,std::ios::binary);
  out << payload; out.close();
  if (!out) throw Error(ErrorCode::io,"Cannot write draft; reserved JOBID requires reconciliation");
  std::filesystem::rename(temporary,file,ec);
  if (ec) throw Error(ErrorCode::io,"Cannot publish draft");
}
void MockController::start() const { throw Error(ErrorCode::hardware_disabled,"Offline prototype cannot start hardware"); }
}
