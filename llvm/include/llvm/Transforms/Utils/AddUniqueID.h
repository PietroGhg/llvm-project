//===- AddUniqueID.h - The -addUnique pass ------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_UTILS_ADDUNIQUE_H
#define LLVM_TRANSFORMS_UTILS_ADDUNIQUE_H

#include "llvm/IR/PassManager.h"
#include "llvm/InitializePasses.h"


class Function;

namespace llvm {

struct AddUniqueIDPass : PassInfoMixin<AddUniqueIDPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};
} //end namespace llvm



#endif 
