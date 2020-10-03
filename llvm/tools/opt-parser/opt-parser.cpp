#include <iostream>
#include <fstream>
#include "llvm/IR/LLVMContext.h"
#include "LogParser.h"
#include "InstrIDMap.h"

using namespace std;

int main(int argc, char* argv[]){
  llvm::LLVMContext Ctx;
  fstream F(argv[1], fstream::in);
  vector<unique_ptr<llvm::Module>> Modules;
  vector<Log> Logs;
  parseLog(F, Ctx, Modules, Logs);
  int I = 0;
  errs() << *Modules[0].get();
  for(auto It = Modules.begin() + 1; It != Modules.end(); It++){
    auto IEM = getInstEntryMap(It->get(), Logs[I]);
    printModule(It->get(), IEM);
    I++;
  }
}
