#include "driver/compiler.h"

#include "front/logger.h"

using namespace mimic::driver;
using namespace mimic::front;

void Compiler::Reset() {
  // reset lexer & parser
  parser_.Reset();
  // reset the rest part
  ana_.Reset();
  eval_.Reset();
}

void Compiler::Open(std::istream *in) {
  // reset lexer & parser only
  lexer_.Reset(in);
  parser_.Reset();
}

bool Compiler::CompileToIR() {
  while (auto ast = parser_.ParseNext()) {
    // perform sematic analyze
    if (!ast->SemaAnalyze(ana_)) break;
    ast->Eval(eval_);
    if (dump_ast_) ast->Dump(*os_);
    // generate IR
    ast->GenerateIR(irb_);
  }
  return !Logger::error_num();
}

bool Compiler::RunPasses() {
  // run passes on IR
  if (dump_pass_info_) pass_man_.ShowInfo(std::cerr);
  irb_.module().RunPasses(pass_man_);
  // check if need to dump IR
  auto err_num = Logger::error_num();
  if (!err_num && dump_yuir_) irb_.module().Dump(*os_);
  return !err_num;
}
