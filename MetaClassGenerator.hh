#ifndef __Generator_MetaClassGenerator_H__
#define __Generator_MetaClassGenerator_H__
#include <clang/Tooling/Refactoring.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

class ClassMembersPrinter;

ClassMembersPrinter& GenerateSerialization(clang::tooling::RefactoringTool& tool, clang::ast_matchers::MatchFinder& finder);
void WriteMetaInfo(ClassMembersPrinter& printer);
#endif
