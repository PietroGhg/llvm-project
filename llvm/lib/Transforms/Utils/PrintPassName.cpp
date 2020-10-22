#include "llvm/ADT/StringRef.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Transforms/Utils/PrintPassName.h"

using namespace llvm;

PrintPassNamePass::PrintPassNamePass() : OS(dbgs()), Name("unknown") {}
PrintPassNamePass::PrintPassNamePass(StringRef Name) : OS(dbgs()), Name(Name) {}
PrintPassNamePass::PrintPassNamePass(StringRef Name,
				     raw_ostream& OS): OS(OS), Name(Name) {}
PreservedAnalyses PrintPassNamePass::run(Module& M, ModuleAnalysisManager& MAM){
  OS << "Running: " << Name << "\n";
  return PreservedAnalyses::all(); 
}

namespace {
  struct PrintPassNameWrapper : public ModulePass {
    static char ID;
    PrintPassNamePass P;
    
    PrintPassNameWrapper() : ModulePass(ID) {}
    PrintPassNameWrapper(StringRef Name) : ModulePass(ID), P(Name) {}
    PrintPassNameWrapper(StringRef Name,
			 raw_ostream& OS) : ModulePass(ID), P(Name, OS){}

    bool runOnModule(Module& M) override{
      ModuleAnalysisManager MAM;
      P.run(M, MAM);
      return false;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }

  StringRef getPassName() const override { return "Logs name of the next running pass"; }
  };
} // end anonymous namespace

char PrintPassNameWrapper::ID = 0;
INITIALIZE_PASS(PrintPassNameWrapper, "printName",
                "Logs name of the next running pass", false, true)

ModulePass *llvm::createPrintPassNamePass(StringRef Name, raw_ostream& OS) {
  return new PrintPassNameWrapper(Name, OS);
}
