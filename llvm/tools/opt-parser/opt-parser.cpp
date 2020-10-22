#include <iostream>
#include <fstream>
#include "llvm/IR/LLVMContext.h"
#include "Log.h"
#include "LogParser.h"
#include "InstrIDMap.h"
#include "RepGraph.h"
#include "HTMLprint.h"

using namespace std;

int main(int argc, char* argv[]){
  llvm::LLVMContext Ctx;
  fstream F(argv[1], fstream::in);
  vector<unique_ptr<llvm::Module>> Modules;
  vector<Log> Logs;
  parseLog(F, Ctx, Modules, Logs);
  IDInstrMap_t Map;
  for(auto& M : Modules){
    updateIDInstrMap(Map, M.get());
  }
  for(auto& el : Modules)
    errs() << *el.get() << "\n";
  for(auto& el : Logs)
    errs() << el.toString() << "\n";
  //auto G = getRepGraph(Logs, Map);
  //printRG(G);
  //propagateLocations(G);
  printLogs(Logs, Modules, argv[2]);
    

}
