#include <iostream>
#include <fstream>
#include "llvm/IR/LLVMContext.h"
#include "LogParser.h"

using namespace std;

int main(int argc, char* argv[]){
  llvm::LLVMContext Ctx;
  fstream F(argv[1], fstream::in);
  parseLog(F, Ctx);
  
}
