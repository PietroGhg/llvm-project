//===- PrintPassName.h - The -printName pass ------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_UTILS_PRINTPASSNAME_H
#define LLVM_TRANSFORMS_UTILS_PRINTPASSNAME_H

#include "llvm/IR/PassManager.h"
#include "llvm/InitializePasses.h"


class Function;

namespace llvm {

struct PrintPassNamePass : PassInfoMixin<PrintPassNamePass> {
  raw_ostream &OS;
  StringRef Name;
  PrintPassNamePass();
  PrintPassNamePass(StringRef Name);
  PrintPassNamePass(StringRef Name, raw_ostream& OS);
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};
} //end namespace llvm



#endif 
