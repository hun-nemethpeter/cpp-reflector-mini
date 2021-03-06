///
/// This file is part of The Pivot framework.
/// Written by Gabriel Dos Reis <gdr@cs.tamu.edu>
/// 

#ifndef IPR_IO_INCLUDED
#define IPR_IO_INCLUDED

#include <algorithm>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "interface.H"

namespace ipr
{
   /// A data structure used to map different uses of a name to the 
   /// corresponding declaration. It is used by XPR printer to print name
   /// disambiguation information. A similar but a bit more elaborated structure
   /// is used by XPR parser to relink uses of names to appropriate declarations.
   /// The key of the map is the node_id of the name used, while the value
   /// is the information about corresponding declaration.
   struct disambiguation_map_type : std::map<int, std::vector<const ipr::Decl*> >
   {
      typedef std::vector<const ipr::Decl*> declarations;

      /// Given a name and a declaration that corresponds to it, looks up
      /// or allocates a disambiguation id for them.
      int get_disambiguation(const ipr::Name& name, const ipr::Decl& declaration)
      {
         declarations& decls = (*this)[name.node_id];
         declarations::const_iterator p = std::find(decls.begin(), decls.end(), &declaration);

         if (p == decls.end())
         {
            decls.push_back(&declaration);
            return decls.size(); ///< Because disambiguations are 1-based
         }
         else
            return p-decls.begin()+1; ///< Because disambiguations are 1-based
      }
   }; ///< of struct disambiguation_map_type

   struct Printer {
      enum Padding {
         None, Before, After
      };

      explicit Printer(std::ostream&);
      
      Padding padding() const { return pad; }

      bool needs_newline() const { return emit_newline; }
      void needs_newline(bool b) { emit_newline = b; }

      void indent(int n) { pending_indentation += n; }
      int indent() const { return pending_indentation; }

      /// This series of declarations cannot be adequately be reduced
      /// into one template declaration, because it would tend to
      /// take over all other good candidates when an implicit conversion
      /// would be needed.
      Printer& operator<<(const char*);

      Printer& operator<<(char c) { stream << c; return *this; }

      Printer& operator<<(signed char c) { stream << c; return *this; }

      Printer& operator<<(unsigned char c) { stream << c; return *this; }

      Printer& operator<<(int i) { stream << i; return *this; }

      template<class T>
      Printer& operator<<(T& f(T&)) { stream << f; return *this; }

      /// Setting padding flags
      Printer& operator<<(Padding p) { pad = p; return *this; }

      void write(const char*, const char*);

      
   private:
      std::ostream& stream;
      Padding pad;
      bool emit_newline;
      int pending_indentation;

   public:
      disambiguation_map_type disambiguation_map;
   };
   

   struct xpr_decl {
      const Expr& decl;
      const bool needs_semicolon; ///< false, in most cases.
      xpr_decl(const Expr& d, bool add_semicolon = false)
            : decl(d), needs_semicolon(add_semicolon)
      { }
   };

   struct xpr_stmt {
      const Expr& stmt;
      const bool needs_semicolon; ///< false, in most cases.
      explicit xpr_stmt(const Expr& s, bool add_semicolon = true) 
            : stmt(s), needs_semicolon(add_semicolon) 
      { }
   };

   struct xpr_type {
      const Type& type;
      explicit xpr_type(const Type& e) : type(e) { }
   };

   struct xpr_expr {
      const Expr& expr;
      explicit xpr_expr(const Expr& e) : expr(e) { }
   };

   Printer& operator<<(Printer&, xpr_decl);
   Printer& operator<<(Printer&, xpr_stmt);
   Printer& operator<<(Printer&, xpr_type);
   Printer& operator<<(Printer&, xpr_expr);
   Printer& operator<<(Printer&, const Unit&);

   Printer& operator<<(Printer&, const Identifier&);
}

#endif ///< IPR_IO_INCLUDED
