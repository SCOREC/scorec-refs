#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <sstream>
#include <cassert>

using StringMap = std::map<std::string, std::string>;
using StringSet = std::set<std::string>;
using StringVector = std::vector<std::string>;

enum FieldValueLimit {
  FVL_NONE,
  FVL_CURLY,
  FVL_QUOTE
};

struct Field {
  Field():limit(FVL_CURLY) {}
  std::string name;
  std::string value;
  FieldValueLimit limit;
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

static void make_lowercase(std::string& s) {
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

static std::string as_lowercase(std::string const& s) {
  std::string s2 = s;
  make_lowercase(s2);
  return s2;
}

static void print_entries(std::ostream& stream, Entries const& entries);

static bool isident(char c) {
  return std::isalnum(c) || c == '-' || c == '_' ||
    c == '.' || c == ':';
}

class Parser {
  ParserState state;
  int line;
  int column;
  int curly_depth;
  bool in_quote;
  Entries entries;
  char c;

  bool is_curly() {
    return c == '{' || c == '}';
  }

  void handle_curly() {
    if (!is_curly()) return;
    if (c == '{') ++curly_depth;
    if (c == '}') --curly_depth;
    if (curly_depth < 0) fail();
  }

  FieldValueLimit& value_limit() {
    return entries.back().fields.back().limit;
  }

  void handle_quote() {
    if (c != '"') return;
    if (value_limit() == FVL_NONE) in_quote = !in_quote;
  }

  void fail() __attribute__((noreturn));

  bool field_value_ended() {
    if (curly_depth != 0) return false;
    switch (value_limit()) {
      case FVL_NONE: return (!in_quote && c == ',') || c == '\n';
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
    while (stream.get(c)) {
      ++column;
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
            state = FIELD_LIMBO;
          } else if (isident(c)) {
            entries.back().key.push_back(c);
          } else fail();
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
          else if (isident(c)) {
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
            value_limit() = FVL_CURLY;
          } else if (c == '"') {
            state = FIELD_VALUE_TEXT;
            value_limit() = FVL_QUOTE;
          } else if (std::isprint(c)) {
            value_limit() = FVL_NONE;
            handle_curly();
            handle_quote();
            entries.back().fields.back().value.push_back(c);
            state = FIELD_VALUE_TEXT;
          } else fail();
        break;
        case FIELD_VALUE_TEXT:
          if (field_value_ended()) {
            state = FIELD_LIMBO;
          } else if (std::isspace(c)) {
            state = FIELD_VALUE_SPACE;
          } else if (std::isprint(c)) {
            handle_curly();
            handle_quote();
            entries.back().fields.back().value.push_back(c);
          } else fail();
        break;
        case FIELD_VALUE_SPACE:
          if (field_value_ended()) {
            state = FIELD_LIMBO;
          } else if (std::isspace(c)) {
            break;
          } else if (std::isprint(c)) {
            handle_curly();
            handle_quote();
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
      if (c == '\n') {
        ++line;
        column = 0;
      }
    }
    if (state != LIMBO) {
      std::cout << "File ended early\n";
      exit(-1);
    }
  }

  Entries& get_entries() { return entries; }

};

void Parser::fail() {
  std::cout << "Parse error at line " << line << " column " << column;
  std::cout << " char " << c;
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
  std::cout << "curly depth " << curly_depth;
  std::cout << " value delimiter ";
  switch (value_limit()) {
    case FVL_NONE: std::cout << "none"; break;
    case FVL_CURLY: std::cout << "}"; break;
    case FVL_QUOTE: std::cout << "\""; break;
  }
  std::cout << '\n';
  print_entries(std::cout, entries);
  exit(-1);
}

static void print_field2(std::ostream& stream, Field const& field) {
  if (field.limit == FVL_NONE) {
    stream << field.name << "=" << field.value;
  } else { /* both quotes and curly braces become curly braces */
    stream << field.name << "={" << field.value << "}";
  }
}

static void print_field(std::ostream& stream, Field const& field, bool last) {
  stream << "  ";
  print_field2(stream, field);
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
  if (entry.type == "string") {
    stream << "@" << entry.type << "{";
    print_field2(stream, entry.fields.back());
    stream << "}\n";
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

/* If an @article entry has a url field, an [Online]
   tag gets printed in the references section.
   I don't want that there, but at the same time we
   may not want to lose the URL, so I'll copy it up
   into a comment above the entry.
 */
static void comment_out_article_urls(Entries& entries) {
  Entry new_entry;
  new_entry.type = "comment";
  for (size_t i = 0; i < entries.size(); ++i) {
    auto& entry = entries[i];
    if (entry.type != "article") continue;
    size_t j;
    for (j = 0; j < entry.fields.size(); ++j) {
      auto const& f = entry.fields[j];
      if (f.name == "url") {
        std::stringstream stream;
        print_field2(stream, f);
        new_entry.comment = stream.str();
        break;
      }
    }
    if (j != entry.fields.size()) {
      entry.fields.erase(entry.fields.begin() + j);
      entries.insert(entries.begin() + i, new_entry);
    }
  }
}

static void remove_fields(Entries& entries, std::string const& name) {
  for (auto& entry : entries) {
    for (auto it = begin(entry.fields); it != end(entry.fields); ++it) {
      if (it->name == name) {
        entry.fields.erase(it);
        break;
      }
    }
  }
}

/* IEEE Editorial Style Manual Appendix D
   https://www.ieee.org/documents/style_manual.pdf */
static char const* const known_abbreviations[][2] = {
{"Acoustics","Acoust."},
{"Administration","Admin."},
{"Administrative","Administ."},
{"American","Amer."},
{"Analysis","Anal."},
{"Annals","Ann."},
{"Annual","Annu."},
{"Apparatus","App."},
{"Applications","Appl."},
{"Applied","Appl."},
{"Association","Assoc."},
{"Automatic","Automat."},
{"British","Brit."},
{"Broadcasting","Broadcast."},
{"Business","Bus."},
{"Canadian","Can."},
{"Chinese","Chin."},
{"Communications","Commun."},
{"Computer","Comput."},
{"Computers","Comput."},
{"Conference","Conf."},
{"Congress","Congr."},
{"Convention","Conv."},
{"Correspondence","Corresp."},
{"Cybernetics","Cybern."},
{"Department","Dept."},
{"Development","Develop."},
{"Digest","Dig."},
{"Economic","Econ."},
{"Economics","Econ."},
{"Education","Edu."},
{"Electrical","Elect."},
{"Electronic","Electron."},
{"Engineering","Eng."},
{"Ergonomics","Ergonom."},
{"European","Eur."},
{"Evolutionary","Evol."},
{"Foundation","Found."},
{"Geoscience","Geosci."},
{"Graphics","Graph."},
{"Industrial","Ind."},
{"Information","Inform."},
{"Institute","Inst."},
{"Intelligence","Intell."},
{"International","Int."},
{"Japan","Jpn."},
{"Journal","J."},
{"Letter","Lett."},
{"Letters","Lett."},
{"Machine","Mach."},
{"Magazine","Mag."},
{"Management","Manage."},
{"Managing","Manag."},
{"Mathematical","Math."},
{"Mechanical","Mech."},
{"Meeting","Meeting"},
{"National","Nat."},
{"Newsletter","Newslett."},
{"Nuclear","Nucl."},
{"Occupation","Occupat."},
{"Operational","Oper."},
{"Optical","Opt."},
{"Optics","Opt."},
{"Organization","Org."},
{"Philosophical","Philosoph."},
{"Proceedings","Proc."},
{"Processing","Process."},
{"Production","Prod."},
{"Productivity","Productiv."},
{"Quarterly","Quart."},
{"Record","Rec."},
{"Reliability","Rel."},
{"Report","Rep."},
{"Research","Res."},
{"Review","Rev."},
{"Royal","Roy."},
{"Science","Sci."},
{"Selected","Select."},
{"Society","Soc."},
{"Sociological","Sociol."},
{"Statistics","Statist."},
{"Studies","Stud."},
{"Supplement","Suppl."},
{"Symposium","Symp."},
{"Systems","Syst."},
{"Technical","Tech."},
{"Techniques","Techn."},
{"Technology","Technol."},
{"Telecommunications","Telecommun."},
{"Transactions","Trans."},
{"Vehicular","Veh."},
{"Working","Work."}
};

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

static StringMap get_abbreviations() {
  StringMap m;
  for (size_t i = 0; i < ARRAY_SIZE(known_abbreviations); ++i) {
    m[known_abbreviations[i][0]] = known_abbreviations[i][1];
  }
  return m;
}

/* whether this character is to be treated similar
   to its own word in text.
   the only different treatment this will get is
   that no space will be put between it and the
   previous word
 */
static bool is_ctrl_word(char c) {
  return c == ',' || c == ':';
}
static bool is_ctrl_word(std::string const& word) {
  return word.size() == 1 && is_ctrl_word(word[0]);
}

static StringVector split_text(std::string const& s) {
  StringVector v;
  bool in_space = true;
  for (auto c : s) {
    bool c_is_space = std::isspace(c);
    if ((in_space && !c_is_space) || is_ctrl_word(c)) {
      v.push_back(std::string());
    }
    if (!c_is_space) {
      v.back().push_back(c);
    }
    if (c_is_space || is_ctrl_word(c)) in_space = true;
    else in_space = false;
  }
  return v;
}

static std::string unsplit_text(StringVector const& v) {
  bool first = true;
  std::string s;
  for (auto const& word : v) {
    if (!first && !is_ctrl_word(word)) s.push_back(' ');
    s += word;
    if (first) first = false;
  }
  return s;
}

static void abbreviate(Entries& entries) {
  StringSet text_fields;
  text_fields.insert(std::string("booktitle"));
  text_fields.insert(std::string("journal"));
  auto abbrevs = get_abbreviations();
  for (auto& entry : entries) {
    auto is_string = entry.type == "string";
    for (auto& field : entry.fields) {
      if (text_fields.count(field.name) || is_string) {
        auto words = split_text(field.value);
        for (auto& word : words) {
          auto it = abbrevs.find(word);
          if (it != abbrevs.end()) word = it->second;
        }
        field.value = unsplit_text(words);
      }
    }
  }
}

int main(int argc, char** argv) {
  bool inplace = false;
  const char* inpath = nullptr;
  const char* outpath = nullptr;
  for (int i = 1; i < argc; ++i) {
    if (std::string("-i") == argv[i]) inplace = true;
    else if (!inpath) inpath = argv[i];
    else if (!outpath) outpath = argv[i];
  }
  if (inplace) outpath = inpath;
  if (!inpath || !outpath) {
    std::cout << "usage: " << argv[0] << " input.bib output.bib\n";
    std::cout << "       " << argv[0] << " -i inout.bib\n";
    return -1;
  }
  Parser parser;
  {
    std::ifstream file(inpath);
    if (!file.is_open()) {
      std::cout << "could not open " << inpath << " for reading\n";
      return -1;
    }
    parser.run(file);
  }
  auto& entries = parser.get_entries();
  comment_out_article_urls(entries);
  remove_fields(entries, "file");
  remove_fields(entries, "abstract");
  abbreviate(entries);
  {
    std::ofstream file(outpath);
    if (!file.is_open()) {
      std::cout << "could not open " << outpath << " for writing\n";
      return -1;
    }
    print_entries(file, entries);
  }
}
