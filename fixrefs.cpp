#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <fstream>

struct Field {
  std::string name;
  std::string value;
};

using Fields = std::vector<Field>;

struct Entry {
  std::string type;
  std::string key;
  Fields fields;
  std::string comment;
};

using Entries = std::vector<Entry>;

enum ParserState {
  LIMBO,
  ENTRY_TYPE,
  ENTRY_KEY,
  FIELD_LIMBO,
  FIELD_NAME,
  FIELD_POST_EQUAL,
  FIELD_VALUE_TEXT,
  FIELD_VALUE_SPACE,
  COMMENT
};

enum FieldValueLimit {
  FVL_NONE,
  FVL_CURLY,
  FVL_QUOTE
};

static void make_lowercase(std::string& s) {
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

static std::string as_lowercase(std::string const& s) {
  std::string s2 = s;
  make_lowercase(s2);
  return s2;
}

static void print_entries(std::ostream& stream, Entries const& entries);

class Parser {
  ParserState state;
  int line;
  int column;
  int curly_depth;
  FieldValueLimit value_limit;
  Entries entries;

  bool is_curly(char c) {
    return c == '{' || c == '}';
  }

  void handle_curly(char c) {
    if (c == '{') ++curly_depth;
    if (c == '}') --curly_depth;
    if (curly_depth < 0) fail();
  }

  void fail() __attribute__((noreturn));

  bool field_value_ended(char c) {
    if (curly_depth != 0) return false;
    switch (value_limit) {
      case FVL_NONE: return c == ',';
      case FVL_CURLY: return c == '}';
      case FVL_QUOTE: return c == '"';
    }
  }

public:

  void run(std::istream& stream) {
    entries.clear();
    state = LIMBO;
    line = 1;
    column = 0;
    curly_depth = 0;
    char c;
    while (stream.get(c)) {
      if (c == '\n') {
        ++line;
        column = 0;
      } else {
        ++column;
      }
      switch (state) {
        case LIMBO:
          if (std::isspace(c)) break;
          else if (c == '@') {
            state = ENTRY_TYPE;
            entries.push_back(Entry());
          } else {
            entries.push_back(Entry());
            entries.back().type = "comment";
            entries.back().comment.push_back(c);
            state = COMMENT;
          }
        break;
        case ENTRY_TYPE:
          if (std::isspace(c)) {
            if (as_lowercase(entries.back().type) == "comment") {
              make_lowercase(entries.back().type);
              state = COMMENT;
            } else break;
          }
          else if (c == '{') {
            make_lowercase(entries.back().type);
            if (entries.back().type == "string") state = FIELD_LIMBO;
            else state = ENTRY_KEY;
          }
          else if (std::isalpha(c)) {
            entries.back().type.push_back(c);
          }
          else fail();
        break;
        case ENTRY_KEY:
          if (std::isspace(c)) break;
          else if (c == ',') {
            make_lowercase(entries.back().key);
            state = FIELD_LIMBO;
          }
          else if (std::isalnum(c)) {
            entries.back().key.push_back(c);
          }
          else fail();
        break;
        case FIELD_LIMBO:
          if (std::isspace(c)) break;
          else if (std::isalpha(c)) {
            entries.back().fields.push_back(Field());
            entries.back().fields.back().name.push_back(c);
            state = FIELD_NAME;
          } else if (c == '}') state = LIMBO;
          else if (c == ',') break;
          else fail();
        break;
        case FIELD_NAME:
          if (std::isspace(c)) break;
          else if (std::isalpha(c)) {
            entries.back().fields.back().name.push_back(c);
          } else if (c == '=') {
            make_lowercase(entries.back().fields.back().name);
            state = FIELD_POST_EQUAL;
          } else fail();
        break;
        case FIELD_POST_EQUAL:
          if (std::isspace(c)) break;
          else if (c == '{') {
            state = FIELD_VALUE_TEXT;
            value_limit = FVL_CURLY;
          } else if (c == '"') {
            state = FIELD_VALUE_TEXT;
            value_limit = FVL_QUOTE;
          } else if (std::isprint(c)) {
            value_limit = FVL_NONE;
            if (is_curly(c)) handle_curly(c);
            entries.back().fields.back().value.push_back(c);
            state = FIELD_VALUE_TEXT;
          } else fail();
        break;
        case FIELD_VALUE_TEXT:
          if (std::isspace(c)) state = FIELD_VALUE_SPACE;
          else if (field_value_ended(c)) {
            state = FIELD_LIMBO;
          } else if (std::isprint(c)) {
            if (is_curly(c)) handle_curly(c);
            entries.back().fields.back().value.push_back(c);
          } else fail();
        break;
        case FIELD_VALUE_SPACE:
          if (std::isspace(c)) break;
          else if (field_value_ended(c)) {
            state = FIELD_LIMBO;
          } else if (std::isprint(c)) {
            if (is_curly(c)) handle_curly(c);
            entries.back().fields.back().value.push_back(' ');
            entries.back().fields.back().value.push_back(c);
            state = FIELD_VALUE_TEXT;
          } else fail();
        break;
        case COMMENT:
          if (c == '\n') state = LIMBO;
          else entries.back().comment.push_back(c);
        break;
      }
    }
    if (state != LIMBO) {
      std::cout << "File ended early\n";
      exit(-1);
    }
  }

};

void Parser::fail() {
  std::cout << "Parse error at line " << line << " column " << column;
  std::cout << " state \"";
  switch (state) {
    case LIMBO: std::cout << "limbo"; break;
    case ENTRY_TYPE: std::cout << "entry type"; break;
    case ENTRY_KEY: std::cout << "entry key"; break;
    case FIELD_LIMBO: std::cout << "field limbo"; break;
    case FIELD_NAME: std::cout << "field name"; break;
    case FIELD_POST_EQUAL: std::cout << "field post equal"; break;
    case FIELD_VALUE_TEXT: std::cout << "field value text"; break;
    case FIELD_VALUE_SPACE: std::cout << "field value space"; break;
    case COMMENT: std::cout << "comment"; break;
  }
  std::cout << "\"\n";
  print_entries(std::cout, entries);
  exit(-1);
}

static void print_field(std::ostream& stream, Field const& field, bool last) {
  stream << "  " << field.name << "={" << field.value << "}";
  if (!last) stream << ",";
  stream << '\n';
}

static void print_fields(std::ostream& stream, Fields const& fields) {
  if (fields.empty()) return;
  for (size_t i = 0; i < fields.size() - 1; ++i)
    print_field(stream, fields[i], false);
  print_field(stream, fields.back(), true);
}

static void print_entry(std::ostream& stream, Entry const& entry) {
  if (entry.type == "comment") {
    stream << entry.comment << '\n';
    return;
  }
  stream << "@" << entry.type << "{";
  if (entry.type != "string") stream << entry.key << ",";
  stream << "\n";
  print_fields(stream, entry.fields);
  stream << "}\n\n";
}

static void print_entries(std::ostream& stream, Entries const& entries) {
  for (auto entry : entries) print_entry(stream, entry);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "usage: " << argv[0] << " input.bib\n";
    return -1;
  }
  std::ifstream file(argv[1]);
  Parser parser;
  parser.run(file);
}
