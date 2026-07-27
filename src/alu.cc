#include <iostream>

#include <z3++.h>

#include <ilang/ilang++.h>

using namespace ilang;

constexpr int XLEN = 8;
constexpr int AW = 5;

constexpr uint64_t FN_ADD = 0;  // funct 0 -> add
constexpr uint64_t FN_SUB = 1;  // funct 1 -> sub

int main() {

  Ila m("alu");

  auto funct = m.NewBvInput("funct", 1);  // 0 = add, 1 = sub
  auto rd = m.NewBvInput("rd", AW);       // destination register index
  auto rs1 = m.NewBvInput("rs1", AW);     // source register 1 index
  auto rs2 = m.NewBvInput("rs2", AW);     // source register 2 index

  // model register file as an uninterpreted function : BV(XLEN) -> BV(AW)
  FuncRef rf("rf", SortRef::BV(XLEN), SortRef::BV(AW));

  // state to keep track of register file writes
  auto wb_addr = m.NewBvState("wb_addr", AW);
  auto wb_data = m.NewBvState("wb_data", XLEN);

  auto v1 = rf(rs1);  // read source 1 through the uninterpreted regfile
  auto v2 = rf(rs2);  // read source 2 through the uninterpreted regfile

  {  // ADD:  rf[rd] <- rf[rs1] + rf[rs2]
    auto i = m.NewInstr("add");
    i.SetDecode(funct == FN_ADD);
    i.SetUpdate(wb_addr, rd);
    i.SetUpdate(wb_data, v1 + v2);
  }

  {  // SUB:  rf[rd] <- rf[rs1] - rf[rs2]
    auto i = m.NewInstr("sub");
    i.SetDecode(funct == FN_SUB);
    i.SetUpdate(wb_addr, rd);
    i.SetUpdate(wb_data, v1 - v2);
  }

  std::cout << "built ILA '" << m.name() << "': " << m.state_num()
            << " states, " << m.instr_num() << " instr\n";

  // ----- 1-step concrete execution -----------------------------------------
  // Scenario:  ADD x3, x1, x2   with   rf[1] = 5, rf[2] = 7   =>   x3 = 12.
  z3::context c;
  IlaZ3Unroller u(c);

  // Fix the decoded fields and the two register-file entries the instruction reads, so the single step is fully concrete
  u.AddStepPred(0, funct == BvConst(FN_ADD, 1));            // add
  u.AddStepPred(0, rd == BvConst(3, AW));                   // dest  = x3
  u.AddStepPred(0, rs1 == BvConst(1, AW));                  // src1  = x1
  u.AddStepPred(0, rs2 == BvConst(2, AW));                  // src2  = x2
  u.AddStepPred(0, rf(BvConst(1, AW)) == BvConst(5, XLEN)); // rf[1] = 5
  u.AddStepPred(0, rf(BvConst(2, AW)) == BvConst(7, XLEN)); // rf[2] = 7

  auto tr = u.UnrollMonoConn(m, 1);  // one transition: state@0 -> state@1

  z3::solver s(c);
  s.add(tr);
  if (s.check() != z3::sat) {
    std::cout << "unexpected: concrete run is unsatisfiable\n";
    return 1;
  }

  auto mdl = s.get_model();
  auto got_addr = mdl.eval(u.GetZ3Expr(wb_addr, 1), true).get_numeral_uint();
  auto got_data = mdl.eval(u.GetZ3Expr(wb_data, 1), true).get_numeral_uint();

  std::cout << "run: ADD x3,x1,x2 | rf[1]=5 rf[2]=7  ==>  wb_addr=x" << got_addr
            << "  wb_data=" << got_data << "\n";

  bool pass = (got_addr == 3) && (got_data == 12);
  std::cout << (pass ? "PASS: wb_addr==x3 and wb_data==rf[1]+rf[2]==12\n"
                     : "FAIL\n");
  return pass ? 0 : 1;
}
