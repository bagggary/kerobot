#include "kerobot/core.hpp"
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>

using namespace kerobot;
namespace {
void require(bool ok) { if (!ok) throw std::runtime_error("Expectation failed"); }
void rejects(ErrorCode code, const std::function<void()>& f) {
  try { f(); } catch (const Error& e) { require(e.code == code); return; }
  throw std::runtime_error("Expected rejection");
}
JointIntent joint() { return {"joint-v1",{0,0,0},{0.1,0,0},1,true}; }
TestWps wps() { return {"wps-test-v1","test-only",0.005,0.1}; }
}
int main() {
  int passed=0,failed=0;
  auto test=[&](const char* name,const std::function<void()>& f) {
    try { f(); ++passed; std::cout << "PASS " << name << '\n'; }
    catch(const std::exception& e) { ++failed; std::cerr << "FAIL " << name << ": " << e.what() << '\n'; }
  };
  test("millimetres convert to SI",[]{require(std::abs(to_metres(100,"mm")-0.1)<1e-12);});
  test("unknown and nonfinite units rejected",[]{rejects(ErrorCode::invalid_units,[]{to_metres(1,"inch");}); rejects(ErrorCode::invalid_units,[]{to_metres(std::numeric_limits<double>::quiet_NaN(),"m");});});
  test("analytic endpoints and midpoint",[]{auto p=nominal_line(joint(),wps(),2); require(p.points_m.size()==3);require(p.points_m[0].x==0);require(std::abs(p.points_m[1].x-0.05)<1e-12);require(p.points_m[2].x==0.1);});
  test("left and right preserve explicit side",[]{auto j=joint(); auto l=nominal_line(j,wps(),1);j.side=-1;auto r=nominal_line(j,wps(),1);require(l.side_normal.y==1 && r.side_normal.y==-1);});
  test("reverse travel reverses tangent",[]{auto j=joint();j.start_m.x=0.1;j.end_m.x=0;auto p=nominal_line(j,wps(),2);require(p.tangent.x==-1 && p.points_m.front().x==0.1 && p.points_m.back().x==0);});
  test("unconfirmed seam rejected",[]{auto j=joint();j.confirmed=false;rejects(ErrorCode::unconfirmed_joint,[&]{nominal_line(j,wps(),2);});});
  test("degenerate and unsupported geometry rejected",[]{auto j=joint();j.end_m=j.start_m;rejects(ErrorCode::invalid_geometry,[&]{nominal_line(j,wps(),2);});j=joint();j.end_m.y=0.1;rejects(ErrorCode::invalid_geometry,[&]{nominal_line(j,wps(),2);});});
  test("NaN and coordinate overflow rejected",[]{auto j=joint();j.end_m.x=std::numeric_limits<double>::quiet_NaN();rejects(ErrorCode::invalid_geometry,[&]{nominal_line(j,wps(),2);});j.start_m.x=-std::numeric_limits<double>::max();j.end_m.x=std::numeric_limits<double>::max();rejects(ErrorCode::invalid_geometry,[&]{nominal_line(j,wps(),2);});});
  test("side and sample bounds enforced",[]{auto j=joint();j.side=0;rejects(ErrorCode::invalid_geometry,[&]{nominal_line(j,wps(),2);});rejects(ErrorCode::invalid_geometry,[]{nominal_line(joint(),wps(),0);});rejects(ErrorCode::invalid_geometry,[]{nominal_line(joint(),wps(),10001);});});
  test("released WPS cannot enter synthetic workflow",[]{auto w=wps();w.status="released";rejects(ErrorCode::invalid_wps,[&]{nominal_line(joint(),w,2);});});
  test("speed domains distinct and bounded",[]{auto w=wps();w.travel_speed_m_s=0.1;rejects(ErrorCode::invalid_wps,[&]{nominal_line(joint(),w,2);});w=wps();w.wire_feed_speed_m_s=std::numeric_limits<double>::infinity();rejects(ErrorCode::invalid_wps,[&]{nominal_line(joint(),w,2);});});
  test("path traversal and JSON injection IDs rejected",[]{rejects(ErrorCode::invalid_id,[]{draft_json("../bad",nominal_line(joint(),wps(),2));});rejects(ErrorCode::invalid_id,[]{draft_json("bad\"id",nominal_line(joint(),wps(),2));});});
  test("draft permanently labeled non-release",[]{auto s=draft_json("JOB-1",nominal_line(joint(),wps(),2));require(s.find("\"release_eligible\":false")!=std::string::npos);require(s.find("\"synthetic\":true")!=std::string::npos);});
  test("persist replay and changed draft conflict",[]{
    auto dir=std::filesystem::temp_directory_path()/("kerobot-test-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    try {
      auto s=draft_json("JOB-1",nominal_line(joint(),wps(),2));persist_draft(dir,"JOB-1",s);persist_draft(dir,"JOB-1",s);
      rejects(ErrorCode::conflict,[&]{persist_draft(dir,"JOB-1",s+" ");});
      std::filesystem::create_directory(dir/"JOB-2");rejects(ErrorCode::conflict,[&]{persist_draft(dir,"JOB-2",s);});
    } catch (...) {std::filesystem::remove_all(dir);throw;}
    std::filesystem::remove_all(dir);
  });
  test("serialization refuses nonfinite process data",[]{auto p=nominal_line(joint(),wps(),2);p.travel_speed_m_s=std::numeric_limits<double>::quiet_NaN();rejects(ErrorCode::invalid_wps,[&]{draft_json("JOB-1",p);});});
  test("mock cannot start motion",[]{MockController m;require(!m.supports_motion());rejects(ErrorCode::hardware_disabled,[&]{m.start();});});
  std::cout << passed << " passed; " << failed << " failed\n";
  return failed ? 1 : 0;
}
