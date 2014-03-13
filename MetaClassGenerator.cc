#include <iostream>
#include <sstream>
#include <fstream>

#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Lex/Lexer.h>
#include <clang/Tooling/Refactoring.h>
#include <clang/Tooling/Tooling.h>

#include <ipr/impl.H>
#include <ipr/io.H>

#include "ClangSourceManagerHelper.hh"

using namespace clang;
using namespace clang::ast_matchers;
using namespace llvm;
using namespace ipr;

// =====================================================================================================================
class ClassMembersPrinter : public ast_matchers::MatchFinder::MatchCallback
{
public:
   ClassMembersPrinter() {}

  virtual void run(const ast_matchers::MatchFinder::MatchResult &Result)
  {
    SM = Result.SourceManager;
    processClassFields(Result.Nodes.getStmtAs<CXXRecordDecl>("classDecl"));
    processEnumFields(Result.Nodes.getStmtAs<EnumDecl>("enumDecl"));
  }

  // -------------------------------------------------------------------------------------------------------------------
  void writeMetaInfo()
  {
    std::cout << "Generated from " << fileName << ". Do not edit by hand.\n\n";
    std::cout << "Classes:\n"
                 "========" << classStream.str() << std::endl;;
    std::cout << "Enums:\n"
                 "======" << enumStream.str() << std::endl;
    std::cout << "Iprs:\n"
                 "======\n" << iprStream.str() << std::endl;
  }

private:

  // -------------------------------------------------------------------------------------------------------------------
  void processClassFields(const CXXRecordDecl* classDecl)
  {
    if (!classDecl)
      return;

    std::pair<FileID, unsigned> fileId = SM->getDecomposedLoc(classDecl->getLocStart());
    if (fileId.first != SM->getMainFileID())
      return;

    fileName = SM->getFilename(classDecl->getLocStart());
    std::string class_text = get_text(*SM, *classDecl);
    if (!classDecl->hasDefinition())
      return;
  
    printClassFields(classDecl);
  }

  // -------------------------------------------------------------------------------------------------------------------
  void printClassFields(const CXXRecordDecl* classDecl)
  {
    classStream << "\nclass: " << classDecl->getQualifiedNameAsString() << "\n";
    for (auto it = classDecl->field_begin(); it != classDecl->field_end(); it++)
    {
      if (it != classDecl->field_begin())
        classStream << ",\n";
      QualType qtype = (*it)->getType();
      classStream << (*it)->getFieldIndex() << "," << qtype.getAsString() << "," << (*it)->getNameAsString();
    }
    classStream << "\n";
  }
  // -------------------------------------------------------------------------------------------------------------------
  void processEnumFields(const EnumDecl* enumDecl)
  {
    if (!enumDecl)
      return;

    std::pair<FileID, unsigned> fileId = SM->getDecomposedLoc(enumDecl->getLocStart());
    if (fileId.first != SM->getMainFileID())
      return;
    printEnumFields(enumDecl);
    createIprEnums(enumDecl);
  }
  // -------------------------------------------------------------------------------------------------------------------
  void createIprEnums(const EnumDecl* enumDecl)
  {
    impl::Enum* iprEnum = unit.make_enum(*unit.global_region());
    iprEnum->id = unit.make_identifier(unit.get_string(enumDecl->getNameAsString()));

    for (auto it = enumDecl->enumerator_begin(); it != enumDecl->enumerator_end(); it++)
    {
       impl::Enumerator* enumerator = iprEnum->add_member(*unit.make_identifier(unit.get_string((*it)->getName().data())));
       enumerator->init = unit.make_literal(unit.get_int(), unit.get_string(std::to_string((*it)->getInitVal().getSExtValue())));
    }
    Printer printer(iprStream);
      printer << "enum: " << *iprEnum << "\n";
    for (const auto& enumerator : iprEnum->members())
      printer << " " << enumerator.name() << "\n";
  }

  // -------------------------------------------------------------------------------------------------------------------
  void printEnumFields(const EnumDecl* enumDecl)
  {
    std::string enumName = enumDecl->getNameAsString();
    std::string scope;
    if (enumDecl->isScoped())
    {
      scope += enumName;
      scope += "::";
    }
    enumStream << "\nenum: " << enumName << "\n";

    for (auto it = enumDecl->enumerator_begin(); it != enumDecl->enumerator_end(); it++)
    {
      enumStream << scope << (*it)->getName().data() << ", " << (*it)->getInitVal().getSExtValue() << "\n";
    }
    enumStream << "\n";
  }

  SourceManager* SM;
  std::string fileName;
  std::stringstream classStream;
  std::stringstream enumStream;
  std::stringstream iprStream;
  impl::Unit unit;
};

// =====================================================================================================================
ClassMembersPrinter& GenerateSerialization(tooling::RefactoringTool& tool, ast_matchers::MatchFinder& finder)
{
  static ClassMembersPrinter classMembersPrinter;
  finder.addMatcher(recordDecl().bind("classDecl"), &classMembersPrinter);
  finder.addMatcher(enumDecl().bind("enumDecl"), &classMembersPrinter);

  return classMembersPrinter;
}

// =====================================================================================================================
void WriteMetaInfo(ClassMembersPrinter& printer)
{
  printer.writeMetaInfo();
}
