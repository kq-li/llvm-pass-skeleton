#include <set>
#include <vector>
#include "llvm/Pass.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
using namespace llvm;

namespace {
  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    bool isNonzeroValue(std::set<Value*> nonzeros, Value* value) {
      if (auto* constant = dyn_cast<Constant>(value)) {
        return !constant->isZeroValue();
      } else if (auto* load = dyn_cast<LoadInst>(value)) {
        return nonzeros.count(load->getPointerOperand());
      } else {
        return nonzeros.count(value);
      }
    }

    virtual bool runOnFunction(Function &F) {
      LLVMContext& Ctx = F.getContext();
      FunctionCallee guardedDiv =
        F.getParent()->getOrInsertFunction("guarded_div",
                                           Type::getInt32Ty(Ctx),
                                           Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx));
      std::set<Value*> nonzeros;
      std::vector<Instruction*> toReplace;
      for (auto& B : F) {
        for (auto& I : B) {
          if (auto* store = dyn_cast_or_null<StoreInst>(&I)) {
            if (this->isNonzeroValue(nonzeros, store->getValueOperand())) {
              nonzeros.insert(store->getPointerOperand());
            } else {
              nonzeros.erase(store->getPointerOperand());
            }
          } else if (I.getOpcode() == Instruction::SDiv) {
            if (!this->isNonzeroValue(nonzeros, I.getOperand(1))) {
              toReplace.push_back(&I);
            }
          }
        }
      }
      for (Instruction* I : toReplace) {
        CallInst* call = CallInst::Create(guardedDiv, {I->getOperand(0), I->getOperand(1)});
        ReplaceInstWithInst(I, call);
      }
      return false;
    }
  };
}

char SkeletonPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new SkeletonPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerSkeletonPass);
