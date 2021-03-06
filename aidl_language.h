#ifndef AIDL_AIDL_LANGUAGE_H_
#define AIDL_AIDL_LANGUAGE_H_

#include <memory>
#include <string>
#include <vector>

#include <base/macros.h>
#include <base/strings.h>

#include <io_delegate.h>

struct yy_buffer_state;
typedef yy_buffer_state* YY_BUFFER_STATE;

class AidlToken {
 public:
  AidlToken(const std::string& text, const std::string& comments);

  const std::string& GetText() const { return text_; }
  const std::string& GetComments() const { return comments_; }

 private:
  std::string text_;
  std::string comments_;

  DISALLOW_COPY_AND_ASSIGN(AidlToken);
};

class AidlNode {
 public:
  AidlNode() = default;
  virtual ~AidlNode() = default;

 private:
  DISALLOW_COPY_AND_ASSIGN(AidlNode);
};

class AidlType : public AidlNode {
 public:
  AidlType(const std::string& name, unsigned line,
           const std::string& comments, bool is_array);
  virtual ~AidlType() = default;

  const std::string& GetName() const { return name_; }
  unsigned GetLine() const { return line_; }
  bool IsArray() const { return is_array_; }
  const std::string& GetComments() const { return comments_; }

  std::string ToString() const;

 private:
  std::string name_;
  unsigned line_;
  bool is_array_;
  std::string comments_;

  DISALLOW_COPY_AND_ASSIGN(AidlType);
};

class AidlArgument : public AidlNode {
 public:
  enum Direction { IN_DIR = 1, OUT_DIR = 2, INOUT_DIR = 3 };

  AidlArgument(AidlArgument::Direction direction, AidlType* type,
               std::string name, unsigned line);
  AidlArgument(AidlType* type, std::string name, unsigned line);
  virtual ~AidlArgument() = default;

  Direction GetDirection() const { return direction_; }
  bool IsOut() const { return direction_ & OUT_DIR; }
  bool IsIn() const { return direction_ & IN_DIR; }
  bool DirectionWasSpecified() const { return direction_specified_; }

  std::string GetName() const { return name_; }
  int GetLine() const { return line_; }
  const AidlType& GetType() const { return *type_; }

  std::string ToString() const;

 private:
  std::unique_ptr<AidlType> type_;
  Direction direction_;
  bool direction_specified_;
  std::string name_;
  unsigned line_;

  DISALLOW_COPY_AND_ASSIGN(AidlArgument);
};

class AidlMethod {
 public:
  AidlMethod(bool oneway, AidlType* type, std::string name,
             std::vector<std::unique_ptr<AidlArgument>>* args,
             unsigned line, const std::string& comments);
  AidlMethod(bool oneway, AidlType* type, std::string name,
             std::vector<std::unique_ptr<AidlArgument>>* args,
             unsigned line, const std::string& comments, int id);
  virtual ~AidlMethod() = default;

  const std::string& GetComments() const { return comments_; }
  const AidlType& GetType() const { return *type_; }
  bool IsOneway() const { return oneway_; }
  const std::string& GetName() const { return name_; }
  unsigned GetLine() const { return line_; }
  bool HasId() const { return has_id_; }
  int GetId() { return id_; }
  void SetId(unsigned id) { id_ = id; }

  const std::vector<std::unique_ptr<AidlArgument>>& GetArguments() const {
    return arguments_;
  }
  // An inout parameter will appear in both GetInArguments()
  // and GetOutArguments().  AidlMethod retains ownership of the argument
  // pointers returned in this way.
  const std::vector<const AidlArgument*>& GetInArguments() const {
    return in_arguments_;
  }
  const std::vector<const AidlArgument*>& GetOutArguments() const {
    return out_arguments_;
  }

 private:
  bool oneway_;
  std::string comments_;
  std::unique_ptr<AidlType> type_;
  std::string name_;
  unsigned line_;
  const std::vector<std::unique_ptr<AidlArgument>> arguments_;
  std::vector<const AidlArgument*> in_arguments_;
  std::vector<const AidlArgument*> out_arguments_;
  bool has_id_;
  int id_;

  DISALLOW_COPY_AND_ASSIGN(AidlMethod);
};

enum {
  USER_DATA_TYPE = 12,
  INTERFACE_TYPE_BINDER
};

class AidlDocumentItem : public AidlNode {
 public:
  AidlDocumentItem() = default;
  virtual ~AidlDocumentItem() = default;

  unsigned item_type;

 private:
  DISALLOW_COPY_AND_ASSIGN(AidlDocumentItem);
};

class AidlQualifiedName : public AidlNode {
 public:
  AidlQualifiedName(std::string term, std::string comments);
  virtual ~AidlQualifiedName() = default;

  const std::vector<std::string>& GetTerms() const { return terms_; }
  const std::string& GetComments() const { return comments_; }
  std::string GetDotName() const { return android::base::Join(terms_, '.'); }

  void AddTerm(std::string term);

 private:
  std::vector<std::string> terms_;
  std::string comments_;

  DISALLOW_COPY_AND_ASSIGN(AidlQualifiedName);
};

class AidlParcelable : public AidlDocumentItem {
 public:
  AidlParcelable(const std::string& name, unsigned line,
                 const std::string& package);
  AidlParcelable(AidlQualifiedName* name, unsigned line,
                 const std::string& package);
  virtual ~AidlParcelable() = default;

  const std::string& GetName() const { return name_; }
  unsigned GetLine() const { return line_; }
  const std::string& GetPackage() const { return package_; }

  AidlParcelable* next = nullptr;
 private:
  std::string name_;
  unsigned line_;
  std::string package_;

  DISALLOW_COPY_AND_ASSIGN(AidlParcelable);
};

class AidlInterface : public AidlDocumentItem {
 public:
  AidlInterface(const std::string& name, unsigned line,
                const std::string& comments, bool oneway_,
                std::vector<std::unique_ptr<AidlMethod>>* methods,
                const std::string& package);
  virtual ~AidlInterface() = default;

  const std::string& GetName() const { return name_; }
  unsigned GetLine() const { return line_; }
  const std::string& GetComments() const { return comments_; }
  bool IsOneway() const { return oneway_; }
  const std::vector<std::unique_ptr<AidlMethod>>& GetMethods() const
      { return methods_; }
  const std::string& GetPackage() const { return package_; }
  std::string GetCanonicalName() const { return package_ + "." + name_; }

 private:
  std::string name_;
  std::string comments_;
  unsigned line_;
  bool oneway_;
  std::vector<std::unique_ptr<AidlMethod>> methods_;
  std::string package_;

  DISALLOW_COPY_AND_ASSIGN(AidlInterface);
};

class AidlImport : public AidlNode {
 public:
  AidlImport(const std::string& from, const std::string& needed_class,
             unsigned line);
  virtual ~AidlImport() = default;

  const std::string& GetFileFrom() const { return from_; }
  const std::string& GetFilename() const { return filename_; }
  const std::string& GetNeededClass() const { return needed_class_; }
  unsigned GetLine() const { return line_; }

  void SetFilename(const std::string& filename) { filename_ = filename; }

 private:
  std::string from_;
  std::string filename_;
  std::string needed_class_;
  unsigned line_;

  DISALLOW_COPY_AND_ASSIGN(AidlImport);
};

class Parser {
 public:
  explicit Parser(const android::aidl::IoDelegate& io_delegate);
  ~Parser();

  // Parse contents of file |filename|.
  bool ParseFile(const std::string& filename);

  void ReportError(const std::string& err, unsigned line);

  bool FoundNoErrors() const { return error_ == 0; }
  const std::string& FileName() const { return filename_; }
  const std::string& Package() const { return package_; }
  void* Scanner() const { return scanner_; }

  void SetDocument(AidlDocumentItem* items) { document_ = items; };

  void AddImport(AidlQualifiedName* name, unsigned line);
  void SetPackage(AidlQualifiedName* name) {
    package_ = name->GetDotName();
    delete name;
  }

  AidlDocumentItem* GetDocument() const { return document_; }
  const std::vector<std::unique_ptr<AidlImport>>& GetImports() { return imports_; }

  void ReleaseImports(std::vector<std::unique_ptr<AidlImport>>* ret) {
      *ret = std::move(imports_);
      imports_.clear();
  }

 private:
  const android::aidl::IoDelegate& io_delegate_;
  int error_ = 0;
  std::string filename_;
  std::string package_;
  void* scanner_ = nullptr;
  AidlDocumentItem* document_ = nullptr;
  std::vector<std::unique_ptr<AidlImport>> imports_;
  std::unique_ptr<std::string> raw_buffer_;
  YY_BUFFER_STATE buffer_;

  DISALLOW_COPY_AND_ASSIGN(Parser);
};

#endif // AIDL_AIDL_LANGUAGE_H_
