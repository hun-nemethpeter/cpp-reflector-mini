#include <iostream>
#include <memory>

#include <clang/Tooling/Refactoring.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Signals.h>

#include "MetaClassGenerator.hh"

using namespace clang;
using namespace llvm;
using namespace tooling;

cl::opt<std::string> build_path(cl::Positional, cl::desc("<build-path>"));
cl::list<std::string> source_paths(cl::Positional, cl::desc("<source0> [... <sourceN>]"), cl::OneOrMore);

  static void
InitCompilationDatabase(std::shared_ptr<CompilationDatabase>& compilations)
{
  if (!compilations)
  {
    std::string error_message;
    compilations.reset(CompilationDatabase::loadFromDirectory(build_path, error_message));
    if (!compilations)
      llvm::report_fatal_error(error_message);
  }
}

int main(int argc, const char **argv)
{
  llvm::sys::PrintStackTraceOnErrorSignal();

  std::shared_ptr<CompilationDatabase> compilations(FixedCompilationDatabase::loadFromCommandLine(argc, argv));
  cl::ParseCommandLineOptions(argc, argv);

  InitCompilationDatabase(compilations);

  tooling::RefactoringTool tool(*compilations, source_paths);
  ast_matchers::MatchFinder finder;

  ClassMembersPrinter& printer = GenerateSerialization(tool, finder);

  std::unique_ptr<clang::tooling::FrontendActionFactory> frontendAction(newFrontendActionFactory(&finder));

  int res = tool.run(frontendAction.get());
  WriteMetaInfo(printer);

  return res;
}
