#ifndef PARSER_H
#define PARSER_H

#include <stack>
#include <sstream>

#include "Types.h"
#include "NumericUtils.h"

namespace PS {
  class Parser {
  public:
    Parser(const char *source, Block *block);
    bool parse();

    std::deque<std::string> &getErrors();

  private:
    char nextFromStream();
    char peekStream() const;

    void beginString();
    void beginBlock();
    void beginWord();

    void endString();
    bool endBlock();
    void endWord();

    void pushError(const char *msg);
    bool isPurelyNumeric(const std::string &str);
    double stringToDouble(const std::string &str);

    unsigned int index;

    // Is the cursor inside a string?
    bool withinString;

    std::ostringstream currentString;
    std::ostringstream currentWord;

    // Store parse errors and warnings
    // (there are no warnings yet)
    std::deque<std::string> errors;

    /**
     * Block Stack. Stores the current block context.
     * Every operation that is read from the input stream is assigned
     * to a block. The top level block is the one that gets executed when
     * the script is run.
     */
    std::stack<Block *> levels;
    std::string source;
  };

  inline Parser::Parser(const char *source, Block *block) : index(0), withinString(false), source(std::string(source)) {
    levels.push(block);
  }

  inline std::deque<std::string> &Parser::getErrors() {
    return this->errors;
  }

  /**
   * '...
   */
  inline void Parser::beginString() {
    currentString.str("");
  }

  /**
   * ...'
   */
  inline void Parser::endString() {
    levels.top()->value.push_back(Operation(Push_OC, new String(currentString.str())));
  }

  inline void Parser::beginWord() {
    currentWord.str("");
  }

  /**
   * @brief determines if the current word is a numeric expression or a call.
   * Creates a push operation if the word was a numeric. Otherwise creates a
   * call operation.
   */
  inline void Parser::endWord() {
    std::string word = currentWord.str();

    if (word.compare("-") == 0) {
      levels.top()->value.push_back(Operation(Minus_OC, 0));
    } else if (word.compare("+") == 0) {
      levels.top()->value.push_back(Operation(Plus_OC, 0));
    } else if (word.compare("dup") == 0) {
      levels.top()->value.push_back(Operation(Dup_OC, 0));
    } else if (word.compare("swap") == 0) {
      levels.top()->value.push_back(Operation(Swap_OC, 0));
    } else if (isPurelyNumeric(word)) {
      levels.top()->value.push_back(Operation(Push_OC, new Number(stringToDouble(word))));
    } else {
      levels.top()->value.push_back(Operation(Call_OC, new Number(Util::NumericUtils::hash(word))));
    }

    beginWord();
  }

  /**
   * {...
   */
  inline void Parser::beginBlock() {
    Block *block = new Block();
    levels.push(block);
  }

  /**
   * ...}
   * @return returns true if the block was properly closed.
   */
  inline bool Parser::endBlock() {
    if (levels.size() <= 1) {
      return false;
    }

    Block *block = levels.top();
    levels.pop();

    Block *parent = levels.top();
    parent->value.push_back(Operation(Push_OC, block));

    return true;
  }

  /**
   * @brief tests if a string can be converted to a numeric value.
   * Currently only numbers and '.' are supported. Negative numbers can be
   * implemented as language features (1 neg).
   *
   * @param the string to test
   * @return true if the string can be converted to a numeric value.
   */
  inline bool Parser::isPurelyNumeric(const std::string& s) {
      std::string::const_iterator it = s.begin();
      bool result = true;
      bool hasDigits = false;

      while (it != s.end()) {
        result &= std::isdigit(*it) || *it == 46;
        if (std::isdigit(*it)) {
          hasDigits = true;
        }
        it++;
      }

      return result && !s.empty() && hasDigits;
  }

  inline double Parser::stringToDouble(const std::string& s) {
    std::istringstream i(s);
    double x;
    if (!(i >> x)) {
      return 0;
    }
    return x;
  }

  /**
   * @brief peek the next item on the input stream without moving the cursor.
   * @return the next char on the input stream.
   */
  inline char Parser::peekStream() const {
    if (index < source.length()) {
      return source.at(index);
    } else {
      return '\0';
    }
  }

  /**
   * @brief return the next item on the input stream and increment the cursor.
   * @return the next char on the input stream.
   */
  inline char Parser::nextFromStream() {
    if (index < source.length()) {
      return source.at(index++);
    } else {
      return '\0';
    }
  }

  /**
   * @brief create an error message in the form <message> (<position in stream>)
   * @param the error message
   */
  inline void Parser::pushError(const char *msg) {
    std::ostringstream ss;
    ss << msg;
    ss << " (at index: ";
    ss << index;
    ss << ")";
    ss << std::endl;
    errors.push_back(ss.str());
  }

  /**
   * @brief parse the input stream and store the operations in the top level block.
   * @return true if the parsing was successful.
   */
  inline bool Parser::parse() {
    beginWord();

    bool commentMode = false;
    for (char next = nextFromStream(); next != 0; next = nextFromStream()) {

      // Comments (#)
      if (next == 35) commentMode = true;
      if (next == 10 || next == 13) commentMode = false;
      if (commentMode) continue;

      switch (next) {
      case 9:
      case 32:
      case 10:
      case 13:
        // Whitespace
        if (withinString) {
          currentString << next;
        } else {
          if (currentWord.tellp() > 0) {
           endWord();
          }
        }
        continue;
      case 39:
        // ' (strings)
        if (withinString) {
          // support for single-quote escape: '''Hello Word!''' => 'Hello World!'
          if (peekStream() == 39) {
            currentString << nextFromStream();
            continue;
          }
        }

        if (currentWord.tellp() > 0) {
          pushError("' not allowed in word name.");
          continue;
        }

        withinString = !withinString;
        if (withinString) {
          beginString();
        } else {
          endString();
        }
        break;
      case 123:
        // { (block start)
        if(!withinString) {
          if (currentWord.tellp() > 0) {
            pushError("{ not allowed in word name");
            break;
          }
          beginBlock();
        } else {
          currentString << next;
        }
        break;
      case 125:
        // } (block end)
        if (!withinString) {
          if (currentWord.tellp() > 0) {
            endWord();
          }

          if (!endBlock()) {
            pushError("Attempted to end a block that hasn't started.");
          }
        } else {
          currentString << next;
        }
        break;
      default:
        // all other characters
        if (withinString) {
          currentString << next;
        } else {
          currentWord << next;
        }
      }

      // Check for errors after each iteration.
      if(errors.size() > 0) {
        return false;
      }
    }

    // Flush the last word in the input stream.
    if (currentWord.tellp() > 0) {
      endWord();
    }

    // For every opening { there must be a closing }
    if (levels.size() > 1) {
      pushError("Unterminated block.");
    }

    return levels.size() == 1;
  }
}

#endif // PARSER_H
