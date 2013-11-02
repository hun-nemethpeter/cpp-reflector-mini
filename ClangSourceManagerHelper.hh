#ifndef HELPER_H__
#define HELPER_H__
#include <string>

#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Lexer.h>

// Returns the text that makes up 'node' in the source.
// Returns an empty string if the text cannot be found.
template <class T> static std::string
get_text(const clang::SourceManager &source_manager, const T &node)
{
  clang::SourceLocation start_spelling_location = source_manager.getSpellingLoc(node.getLocStart());
  clang::SourceLocation end_spelling_location = source_manager.getSpellingLoc(node.getLocEnd());

  if (!start_spelling_location.isValid() || !end_spelling_location.isValid())
    return std::string();

  bool invalid = true;
  const char *text = source_manager.getCharacterData(start_spelling_location, &invalid);

  if (invalid)
    return std::string();

  std::pair<clang::FileID, unsigned> start = source_manager.getDecomposedLoc(start_spelling_location);
  std::pair<clang::FileID, unsigned> end = source_manager.getDecomposedLoc(
      clang::Lexer::getLocForEndOfToken(end_spelling_location, 0, source_manager, clang::LangOptions()));

  if (start.first != end.first)
  {
    // start and end are in different files.
    return std::string();
  }
  if (end.second < start.second)
  {
    // Shuffling text with macros may cause this.
    return std::string();
  }

  return std::string(text, end.second - start.second);
}

template <class T> static unsigned
get_start_pos(const clang::SourceManager& source_manager, const T& node)
{
  return source_manager.getDecomposedLoc(source_manager.getSpellingLoc(node.getLocStart())).second;
}

template <class T> static const char*
get_start_buf(const clang::SourceManager& source_manager, const T& node)
{
  clang::SourceLocation start_spelling_location = source_manager.getSpellingLoc(node.getLocStart());
  bool invalid = true;
  return source_manager.getCharacterData(start_spelling_location, &invalid);
}

template <class T> static unsigned
get_end_pos(const clang::SourceManager& source_manager, const T& node)
{
  return source_manager.getDecomposedLoc(source_manager.getSpellingLoc(node.getLocEnd())).second;
}

template <class T> static const char*
get_end_buf(const clang::SourceManager& source_manager, const T& node)
{
  const char* start_buf = get_start_buf(source_manager, node);
  clang::SourceLocation start_spelling_location = source_manager.getSpellingLoc(node.getLocStart());
  clang::SourceLocation end_spelling_location = source_manager.getSpellingLoc(node.getLocEnd());
  std::pair<clang::FileID, unsigned> start = source_manager.getDecomposedLoc(start_spelling_location);
  std::pair<clang::FileID, unsigned> end = source_manager.getDecomposedLoc(
      clang::Lexer::getLocForEndOfToken(end_spelling_location, 0, source_manager, clang::LangOptions()));

  return start_buf + end.second - start.second;
}
#endif
