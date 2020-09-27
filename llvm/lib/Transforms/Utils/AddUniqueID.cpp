//===- AddUniqueID.cpp - The -addUnique pass ----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Sets unique IDs for each instruction
//
//===----------------------------------------------------------------------===//
#include "llvm/IR/Metadata.h"
#include "llvm/IR/PassManager.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/Transforms/Utils/AddUniqueID.h"
#include "llvm/Transforms/Utils.h"


using namespace llvm;

namespace {
  struct AddUniqueID : public ModulePass {
    static char ID;

    AddUniqueID() : ModulePass(ID) {
      initializeAddUniqueIDPass(*PassRegistry::getPassRegistry());
    }


    bool runOnModule(llvm::Module& M) override{
      for(auto& F : M){
	for(auto& BB : F){
	  for(auto& I : BB){
	    MDNode* N = MDNode::get(M.getContext(), M.getNewID());
	    I.setMetadata("ID", N);
	  }
	}
      }
      return true;
    }
  };
} // end anonymous namespace
  
char AddUniqueID::ID = 0;

INITIALIZE_PASS(AddUniqueID, "addUnique", "Add Unique ID ",
                      false, false)
/*INITIALIZE_PASS_BEGIN(AddUniqueID, "addUnique", "Add Unique ID ",
                      false, false)
INITIALIZE_PASS_END(AddUniqueID, "addUnique", "Add Unique ID ",
                    false, false)*/


ModulePass *llvm::createAddUniqueIDPass() {
  return new AddUniqueID();
}

PreservedAnalyses AddUniqueIDPass::run(llvm::Module &M,
                                        FunctionAnalysisManager &AM) {
  for(auto& F : M){
    for(auto& BB : F){
      for(auto& I : BB){
	MDNode* N = MDNode::get(M.getContext(), M.getNewID());
	I.setMetadata("ID", N);
      }
    }
  }

  return PreservedAnalyses::all();
}
