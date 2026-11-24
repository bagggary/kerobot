#include "kerobot/core.hpp"
#include <iostream>
int main(int argc, char** argv) {
  try {
    if (argc != 2) { std::cerr << "Usage: kerobot_offline <draft-directory>\n"; return 2; }
    const kerobot::JointIntent joint{"synthetic-joint-v1",{0,0,0},{kerobot::to_metres(100,"mm"),0,0},1,true};
    const kerobot::TestWps wps{"synthetic-wps-v1","test-only",0.005,0.1};
    const auto plan=kerobot::nominal_line(joint,wps,10);
    const auto json=kerobot::draft_json("JOB-SYNTHETIC-001",plan);
    kerobot::persist_draft(argv[1],"JOB-SYNTHETIC-001",json);
    std::cout << json;
    return 0;
  } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
