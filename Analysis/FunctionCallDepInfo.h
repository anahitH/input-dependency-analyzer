#pragma once

#include "DependencyInfo.h"

namespace llvm {

class Argument;
class CallInst;
class Function;
class GlobalVariable;
class InvokeInst;
}

namespace input_dependency {

class FunctionCallDepInfo
{
public:
    // TODO: change to ValueDepInfo
    using ArgumentDependenciesMap = std::unordered_map<llvm::Argument*, DepInfo>;
    using GlobalVariableDependencyMap = std::unordered_map<llvm::GlobalVariable*, DepInfo>;
    using CallSiteArgumentsDependenciesMap = std::unordered_map<const llvm::Instruction*, ArgumentDependenciesMap>;
    using CallSiteGloblasDependenciesMap = std::unordered_map<const llvm::Instruction*, GlobalVariableDependencyMap>;

public:
    FunctionCallDepInfo() = default;
    FunctionCallDepInfo(const llvm::Function& F);

public:
    bool empty() const;

    void addCall(const llvm::CallInst* callInst, const ArgumentDependenciesMap& deps);
    void addInvoke(const llvm::InvokeInst* invokeInst, const ArgumentDependenciesMap& deps);
    void addCall(const llvm::CallInst* callInst, const GlobalVariableDependencyMap& deps);
    void addInvoke(const llvm::InvokeInst* invokeInst, const GlobalVariableDependencyMap& deps);

    void addCall(const llvm::Instruction* instr, const ArgumentDependenciesMap& deps);
    void addCall(const llvm::Instruction* instr, const GlobalVariableDependencyMap& deps);
    void addDepInfo(const FunctionCallDepInfo& depInfo);

    void removeCall(const llvm::Instruction* callInst);

    const CallSiteArgumentsDependenciesMap& getCallsArgumentDependencies() const;
    const CallSiteGloblasDependenciesMap& getCallsGlobalsDependencies() const;

    const ArgumentDependenciesMap& getArgumentDependenciesForCall(const llvm::CallInst* callInst) const;
    const ArgumentDependenciesMap& getArgumentDependenciesForInvoke(const llvm::InvokeInst* invokeInst) const;
    const GlobalVariableDependencyMap& getGlobalsDependenciesForCall(const llvm::CallInst* callInst) const;
    const GlobalVariableDependencyMap& getGlobalsDependenciesForInvoke(const llvm::InvokeInst* invokeInst) const;
    ArgumentDependenciesMap& getArgumentsDependencies(const llvm::Instruction* instr);
    GlobalVariableDependencyMap& getGlobalsDependencies(const llvm::Instruction* instr);

    // Used in reflection algorithm.
    ArgumentDependenciesMap& getArgumentDependenciesForCall(const llvm::CallInst* callInst);
    ArgumentDependenciesMap& getArgumentDependenciesForInvoke(const llvm::InvokeInst* invokeInst);
    GlobalVariableDependencyMap& getGlobalsDependenciesForCall(const llvm::CallInst* callInst);
    GlobalVariableDependencyMap& getGlobalsDependenciesForInvoke(const llvm::InvokeInst* invokeInst);

    ArgumentDependenciesMap getMergedArgumentDependencies() const;
    GlobalVariableDependencyMap getMergedGlobalsDependencies() const;

    /**
    * \brief Finalize call instructions dependencies based on actual dependencies of caller.
    * \param actualDeps Argument dependencies map of caller function.
    */
    void finalizeArgumentDependencies(const ArgumentDependenciesMap& actualDeps);

    void finalizeGlobalsDependencies(const GlobalVariableDependencyMap& actualDeps);

    void markAllInputDependent();

private:
    bool isValidInstruction(const llvm::Instruction* instr) const;
    void addCallSiteArguments(const llvm::Instruction* instr, const ArgumentDependenciesMap& argDeps);
    void addCallSiteGlobals(const llvm::Instruction* instr, const GlobalVariableDependencyMap& globalDeps);

    template <class Key>
    void markAllInputDependent(std::unordered_map<Key, DepInfo>& argDeps);

private:
    const llvm::Function* m_F;
    CallSiteArgumentsDependenciesMap m_callsArgumentsDeps;
    CallSiteGloblasDependenciesMap m_callsGlobalsDeps;
};

} // namespace input_dependency

