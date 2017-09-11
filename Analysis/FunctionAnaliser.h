#pragma once

#include "InputDependencyResult.h"
#include "DependencyAnaliser.h"
#include "definitions.h"

#include <memory>

namespace llvm {
class GlobalVariable;
class LoopInfo;
class PostDominatorTree;
}

namespace input_dependency {

class IndirectCallSitesAnalysisResult;
class VirtualCallSiteAnalysisResult;

class FunctionAnaliser : public InputDependencyResult
{
public:
    FunctionAnaliser(llvm::Function* F,
                     llvm::AAResults& AAR,
                     llvm::LoopInfo& LI,
                     const llvm::PostDominatorTree& PDom,
                     const VirtualCallSiteAnalysisResult& virtualCallsInfo,
                     const IndirectCallSitesAnalysisResult& indirectCallsInfo,
                     const FunctionAnalysisGetter& getter);

public:
    void setFunction(llvm::Function* F);
    llvm::Function* getFunction();
    const llvm::Function* getFunction() const;

    /// \name InputDependencyResult interface
    /// \{
public:
    bool isInputDependent(llvm::Instruction* instr) const override;
    bool isInputDependent(const llvm::Instruction* instr) const override;
    bool isInputIndependent(llvm::Instruction* instr) const override;
    bool isInputIndependent(const llvm::Instruction* instr) const override;
    bool isInputDependent(llvm::Value* val) const override;
    bool isInputIndependent(llvm::Value* val) const override;
    bool isInputDependentBlock(llvm::BasicBlock* block) const override;

    // for debug only
    long unsigned get_input_dep_count() const override;
    long unsigned get_input_indep_count() const override;
    long unsigned get_input_unknowns_count() const override;

    FunctionAnaliser* toFunctionAnalysisResult() override
    {
        return this;
    }
    /// \}

    /// \name Analysis interface
    /// \{
public:
    /**
     * \brief Preliminary analyses input dependency of instructions in the function.
     * Performs context insensitive, flow sensitive input dependency analysis
     * Collects function call site dependency info.
     * \note Assumes that function arguments are user inputs.
     */
    void analize();

    /**
     * \brief Refines results of the \link analize by performing context-sensitive analysis given set of input dep arguments.
     * \param[in] inputDepArgs Arguments which are actually input dependent.
     * 
     * \note \link analize function should be called before calling this function.
     */
    void finalizeArguments(const DependencyAnaliser::ArgumentDependenciesMap& inputDepArgs);

    /**
     * \brief Refines results of analysis given set of input dependent globals.
     * \param[in] globalsDeps global variables that are input dependent.
     * \note \link analize function should be called before calling this function.
     */
    void finalizeGlobals(const DependencyAnaliser::GlobalVariableDependencyMap& globalsDeps);

    /// \}

    /// \name Intermediate input dep results interface
    /// \{
    FunctionSet getCallSitesData() const;
    const DependencyAnaliser::ArgumentDependenciesMap& getCallArgumentInfo(llvm::Function* F) const;
    FunctionCallDepInfo getFunctionCallDepInfo(llvm::Function* F) const;
    DependencyAnaliser::GlobalVariableDependencyMap getCallGlobalsInfo(llvm::Function* F) const;
    bool isOutArgInputIndependent(llvm::Argument* arg) const;
    DepInfo getOutArgDependencies(llvm::Argument* arg) const;
    bool isReturnValueInputIndependent() const;
    const DepInfo& getRetValueDependencies() const;
    bool hasGlobalVariableDepInfo(llvm::GlobalVariable* global) const;
    const DepInfo& getGlobalVariableDependencies(llvm::GlobalVariable* global) const;
    DepInfo getDependencyInfoFromBlock(llvm::Value* val, llvm::BasicBlock* block) const;
    const GlobalsSet& getReferencedGlobals() const;
    const GlobalsSet& getModifiedGlobals() const;
    /// \}

    /// \name debug interface
    /// \{
    void dump() const;
    /// \}

private:
    class Impl;
    std::shared_ptr<Impl> m_analiser;
};

} // namespace input_dependency
