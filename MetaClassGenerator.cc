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
                 "=====\n" << iprStream.str() << std::endl;
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
    creatIprClass(classDecl);
  }

  // -------------------------------------------------------------------------------------------------------------------
  void creatIprClass(const CXXRecordDecl* clangClass)
  {
    impl::Class& iprClass = *unit.make_class(*unit.global_region());
    iprClass.id = unit.make_identifier(unit.get_string(clangClass->getNameAsString()));
    unit.global_ns.declare_type(*iprClass.id, unit.get_class())->init = &iprClass;

    for (auto it = clangClass->field_begin(); it != clangClass->field_end(); it++)
    {
      const ipr::Identifier& iprFieldName = unit.get_identifier((*it)->getNameAsString());
      const ipr::Type* iprFieldType = &unit.get_void();
      if (const BuiltinType *BT = dyn_cast<BuiltinType>((*it)->getType()->getCanonicalTypeInternal()))
      {
        // TODO: include/clang/AST/BuiltinTypes.def
        switch (BT->getKind())
        {
          case BuiltinType::Void:
            iprFieldType = &unit.get_void();
            break;
          case BuiltinType::Bool:
            iprFieldType = &unit.get_bool();
            break;
          case BuiltinType::Char_U:
          case BuiltinType::Char_S:
            iprFieldType = &unit.get_char();
            break;
          case BuiltinType::UChar:
            iprFieldType = &unit.get_uchar();
            break;
          case BuiltinType::SChar:
            iprFieldType = &unit.get_schar();
            break;
          case BuiltinType::WChar_U:
          case BuiltinType::WChar_S:
            iprFieldType = &unit.get_wchar_t();
            break;
          case BuiltinType::Short:
            iprFieldType = &unit.get_short();
            break;
          case BuiltinType::UShort:
            iprFieldType = &unit.get_ushort();
            break;
          case BuiltinType::Int:
            iprFieldType = &unit.get_int();
            break;
          case BuiltinType::UInt:
            iprFieldType = &unit.get_uint();
            break;
          case BuiltinType::Long:
            iprFieldType = &unit.get_long();
            break;
          case BuiltinType::ULong:
            iprFieldType = &unit.get_ulong();
            break;
          case BuiltinType::LongLong:
            iprFieldType = &unit.get_long_long();
            break;
          case BuiltinType::ULongLong:
            iprFieldType = &unit.get_ulong_long();
            break;
          case BuiltinType::Float:
            iprFieldType = &unit.get_float();
            break;
          case BuiltinType::Double:
            iprFieldType = &unit.get_double();
            break;
          case BuiltinType::LongDouble:
            iprFieldType = &unit.get_long_double();
            break;
          default:
            break;
        }
      }
      else if ((*it)->getType()->isClassType())
      {
         iprFieldType = &unit.get_class();
      }
      impl::Field* field = iprClass.declare_field(iprFieldName, *iprFieldType);
      field->decl_data.spec = ipr::Decl::Public;
    }
    Printer printer(iprStream);
    printer << "class: " << iprClass.name() << "\n";
    for (const auto& field : iprClass.members())
      printer << "  " << field.type().name() << "," << field.name() << "\n";
  }

  // -------------------------------------------------------------------------------------------------------------------
  void printClassFields(const CXXRecordDecl* classDecl)
  {
    classStream << "\nclass: " << classDecl->getQualifiedNameAsString() << "\n";
    for (auto it = classDecl->field_begin(); it != classDecl->field_end(); it++)
    {
      if (it != classDecl->field_begin())
        classStream << ",\n";
      const QualType& qtype = (*it)->getType();
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
    createIprEnum(enumDecl);
  }
  // -------------------------------------------------------------------------------------------------------------------
  void createIprEnum(const EnumDecl* clangEnum)
  {
    impl::Enum& iprEnum = *unit.make_enum(*unit.global_region());
    iprEnum.id = unit.make_identifier(unit.get_string(clangEnum->getNameAsString()));

    for (auto it = clangEnum->enumerator_begin(); it != clangEnum->enumerator_end(); it++)
    {
       impl::Enumerator* enumerator = iprEnum.add_member(unit.get_identifier((*it)->getName().data()));
       const std::string& enumInitAsStr = std::to_string((*it)->getInitVal().getSExtValue());
       enumerator->init = &unit.get_literal(unit.get_int(), enumInitAsStr);
    }
    Printer printer(iprStream);
      printer << "enum: " << iprEnum.name() << "\n";
    for (const auto& enumerator : iprEnum.members())
      printer << "  " << enumerator.name() << "\n";
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
