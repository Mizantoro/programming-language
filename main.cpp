#include <cmath>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

enum class OperationTypes {
    DISPLAY,
    DEF,
    SET,
    IF,
    END,
    ELSE,
    WHILE,
    KILL,
    INPUT,
};
unordered_map<string, OperationTypes> operations = {
    {"display", OperationTypes::DISPLAY},
    {"def", OperationTypes::DEF},
    {"set", OperationTypes::SET},
    {"if", OperationTypes::IF},
    {"end", OperationTypes::END},
    {"else", OperationTypes::ELSE},
    {"while", OperationTypes::WHILE},
    {"kill", OperationTypes::KILL},
    {"input", OperationTypes::INPUT},
};

enum class DefTypes {
    NUM,
    STR,
    BOOL,
};
unordered_map<string, DefTypes> defTypes = {
    {"num", DefTypes::NUM},
    {"str", DefTypes::STR},
    {"bool", DefTypes::BOOL},
};

enum class DisplayTypes {
    STR,
    VAR,
    ENDL,
};
unordered_map<string, DisplayTypes> displayTypes = {
    {"str", DisplayTypes::STR},
    {"var", DisplayTypes::VAR},
    {"endl", DisplayTypes::ENDL},
};

enum class NumberOperationTypes {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO,
    POWER,
};
const unordered_map<string, NumberOperationTypes> numberOperationTypes = {
    {"add", NumberOperationTypes::ADD},
    {"+", NumberOperationTypes::ADD},
    {"sub", NumberOperationTypes::SUBTRACT},
    {"-", NumberOperationTypes::SUBTRACT},
    {"mul", NumberOperationTypes::MULTIPLY},
    {"*", NumberOperationTypes::MULTIPLY},
    {"div", NumberOperationTypes::DIVIDE},
    {"/", NumberOperationTypes::DIVIDE},
    {"mod", NumberOperationTypes::MODULO},
    {"%", NumberOperationTypes::MODULO},
    {"pow", NumberOperationTypes::POWER},
    {"^", NumberOperationTypes::POWER},
};

enum class IfOperationTypes {
    EQUALS,
    GREATER,
    GREATER_EQUALS,
    LESSER,
    LESSER_EQUALS,
};
const unordered_map<string, IfOperationTypes> ifOperationTypes = {
    {"equals", IfOperationTypes::EQUALS},
    {"==", IfOperationTypes::EQUALS},
    {"greater", IfOperationTypes::GREATER},
    {">", IfOperationTypes::GREATER},
    {"greater_equals", IfOperationTypes::GREATER_EQUALS},
    {">=", IfOperationTypes::GREATER_EQUALS},
    {"lesser", IfOperationTypes::LESSER},
    {"<", IfOperationTypes::LESSER},
    {"lesser_equals", IfOperationTypes::LESSER_EQUALS},
    {"<=", IfOperationTypes::LESSER_EQUALS},
};

// enum class LogicOperators {
//     OR,
//     AND,
// };
// const unordered_map<string, LogicOperators> logicOperators = {
//     {"or", LogicOperators::OR},
//     {"AND", LogicOperators::AND},
// };

unordered_map<string, long long> NumVariables;
unordered_map<string, string> StrVariables;
unordered_map<string, bool> BoolVariables;
// This map is used by 'else' expressions to remember the value of the previous if statement.
// If 'if' statement is called it will overwrite it's value in the map.
unordered_map<unsigned int, bool> ifValues;

vector<string> lines;
vector<OperationTypes> instructions;
vector<string> arguments;

fs::path Path;

string firstWord(string str) {
    return str.substr(0, str.find(' '));
}

string removeFirstWord(string str) {
    return str.erase(0, str.find(' ') + 1);
}

bool isDigits(string str) {
    if (str.empty()) {
        return false;
    }

    for (int i = 0; i < str.length(); i++) {
        if (i == 0 && str[i] == '-') {
            continue;
        }

        if (!isdigit(str[i])) {
            return false;
        }
    }

    return true;
}

bool variableExists(string str) {
    auto numIT  = NumVariables.find(str);
    auto strIT  = StrVariables.find(str);
    auto boolIT = BoolVariables.find(str);

    if (
        numIT  == NumVariables.end() &&
        strIT  == StrVariables.end() &&
        boolIT == BoolVariables.end()
        ) {
            return false;
        }

    return true;
}

long long getNumber(string str) {
    bool negative = false;
    string tmp;

    for (unsigned int i = 0; i < str.length(); i++) {
        if (i == 0 && str[i] == '-') {
            negative = true;
            continue;
        }

        if (str[i] >= '0' && str[i] <= '9') {
            tmp += str[i];
        }
    }

    if (tmp.empty()) {
        cerr << "Error: num variable is not numeric!" << endl;
        exit(4);
    }

    long long value = stoll(tmp);

    if (negative) {
        return -value;
    }

    return value;
}

long long getNumValueFromStringOrMap(string str) {
    long long var;

    if (isDigits(firstWord(str))) {
        var = getNumber(firstWord(str));
    }
    else {
        if (!variableExists(firstWord(str))) {
            cerr << "Error: variable does not exist!" << endl;
            exit(6);
        }
        var = NumVariables[firstWord(str)];
    }

    return var;
}

unsigned int findIntegerOfNestedEnd(unsigned int i) {
    i++;
    unsigned int blocks = 0;
    while (true) {
        if (
            instructions[i] == OperationTypes::IF    ||
            instructions[i] == OperationTypes::WHILE ||
            instructions[i] == OperationTypes::ELSE
            ) {
            blocks++;
        }

        if (instructions[i] == OperationTypes::END) {
            if (blocks > 0) {
                 blocks--;
            }
            else {
                break;
            }
        }
        i++;
    }

    return i;
}

unsigned int findPreviousIf(unsigned int i) {
    i = i - 2;
    unsigned int blocks = 0;
    while (true) {
        if (instructions[i] == OperationTypes::END) {
            blocks++;
        }

        if (
           instructions[i] == OperationTypes::WHILE ||
           instructions[i] == OperationTypes::ELSE
           ) {
            blocks--;
           }

        if (instructions[i] == OperationTypes::IF) {
            if (blocks > 0) {
                blocks--;
            }
            else {
                break;
            }
        }
        i--;
    }

    return i;
}

unsigned int findPreviousWhile(unsigned int i) {
    i--;
    unsigned int blocks = 0;
    while (true) {
        if (instructions[i] == OperationTypes::END) {
            blocks++;
        }

        if (
            instructions[i] == OperationTypes::IF ||
            instructions[i] == OperationTypes::ELSE
            ) {
            blocks--;
        }

        if (instructions[i] == OperationTypes::WHILE) {
            if (blocks > 0) {
                blocks--;
            }
            else {
                break;
            }
        }
        i--;
    }

    return i - 1;
}

void loadProgram() {
    ifstream programFile;

    programFile.open(Path);

    if (!programFile.is_open()) {
        cout << "Error while opening file!" << endl;
        exit(1);
    }

    string line;

    while (getline(programFile, line)) {
        if (
            line[0] == '#' ||
            line == ""     ||
            line == "\n"
            ) {
            continue;
        }
        lines.push_back(line);
    }

    programFile.close();
}

void formatProgramInput() {
    for (unsigned int i = 0; i < lines.size(); i++) {
        lines[i].erase(0, lines[i].find_first_not_of(' '));
    }
}

void translateInputIntoInstructions() {
    for (unsigned int i = 0; i < lines.size(); i++) {
        instructions.push_back(operations[lines[i].substr(0, lines[i].find(' '))]);
        arguments.push_back(removeFirstWord(lines[i]));
    }
}

bool returnIfValue(long long var1, long long var2, IfOperationTypes type) {
    bool expression;

    switch (type) {
        case IfOperationTypes::EQUALS:
            if (var1 == var2) {
                expression = true;
            }
            else {
                expression = false;
            }
            break;

        case IfOperationTypes::GREATER:
            if (var1 > var2) {
                expression = true;
            }
            else {
                expression = false;
            }
            break;

        case IfOperationTypes::GREATER_EQUALS:
            if (var1 >= var2) {
                expression = true;
            }
            else {
                expression = false;
            }
            break;

        case IfOperationTypes::LESSER:
            if (var1 < var2) {
                expression = true;
            }
            else {
                expression = false;
            }
            break;

        case IfOperationTypes::LESSER_EQUALS:
            if (var1 <= var2) {
                expression = true;
            }
            else {
                expression = false;
            }
            break;
    }
    return expression;
}

bool endIsEndOfWhile(unsigned int i) {
    unsigned int blocks = 0;
    while (true) {
        if (instructions[i] == OperationTypes::END) {
            blocks++;
        }

        if (instructions[i] == OperationTypes::IF || instructions[i] == OperationTypes::WHILE) {
            blocks--;
        }

        if (blocks == 0 && instructions[i] == OperationTypes::IF) {
            return false;
        }

        if (i == 0) {
            return false;
        }

        if (instructions[i] == OperationTypes::WHILE) {
            if (blocks > 0) {
                blocks--;
            }
            else {
                break;
            }
        }
        i--;
    }

    return true;
}

void assignVarToMapVariable(string argument, string target) {
    auto numIT  = NumVariables.find(target);
    if (numIT != NumVariables.end()) {
        if (!isDigits(firstWord(argument))) {
            cerr << "Error: cannot assign string value to a numeric variable!" << endl;
            exit(7);
        }
        NumVariables[target] = stoi(firstWord(argument));
    }

    auto strIT  = StrVariables.find(target);
    if (strIT != StrVariables.end()) {
        StrVariables[target] = argument;
    }

    auto boolIT = BoolVariables.find(target);
    if (boolIT != BoolVariables.end()) {
        if (argument != "true" && argument != "false") {
            cerr << "Error: cannot assign string value to a bool variable!" << endl;
            exit(8);
        }
        if (argument == "true") {
            BoolVariables[target] = true;
        }
        else {
            BoolVariables[target] = false;
        }
    }
}

bool conditionsContainsNot(string str) {
    if (str == "not" || str == "!") {
        return true;
    }
    return false;
}

void executeInstructions() {
    cout << "Running: " << Path << endl << endl;
    for (unsigned int i = 0; i < instructions.size(); i++) {
        switch (instructions[i]) {
            case OperationTypes::DISPLAY: {
                DisplayTypes type = displayTypes[firstWord(arguments[i])];
                string argument = arguments[i];
                argument = removeFirstWord(argument);
                switch (type) {
                    case DisplayTypes::STR:
                        cout << argument;
                        break;
                    case DisplayTypes::ENDL:
                        cout << endl;
                        break;
                    case DisplayTypes::VAR:
                        auto numIT  = NumVariables.find(argument);
                        auto strIT  = StrVariables.find(argument);
                        auto boolIT = BoolVariables.find(argument);

                        if (numIT != NumVariables.end()) {
                            cout << NumVariables[argument];
                            break;
                        }
                        if (strIT != StrVariables.end()) {
                            cout << StrVariables[argument];
                            break;
                        }
                        if (boolIT != BoolVariables.end()) {
                            if (BoolVariables[argument] == true) {
                                cout << "true";
                            }
                            else {
                                cout << "false";
                            }
                            break;
                        }

                        cerr << "Error: variable does not exist!";
                        exit(3);
                }
                break;
            }

            case OperationTypes::SET: {
                string argument = arguments[i];

                if (!variableExists(firstWord(argument))) {
                    cerr << "Error: variable does not exist!" << endl;
                    exit(6);
                }

                string target = firstWord(argument);

                argument = removeFirstWord(argument);

                if (numberOperationTypes.find(firstWord(argument)) != numberOperationTypes.end()) {
                    NumberOperationTypes type = numberOperationTypes.at(firstWord(argument));

                    argument = removeFirstWord(argument);

                    long long var1, var2;

                    var1 = getNumValueFromStringOrMap(firstWord(argument));
                    argument = removeFirstWord(argument);
                    var2 = getNumValueFromStringOrMap(firstWord(argument));

                    switch (type) {
                        case NumberOperationTypes::ADD:
                            NumVariables[target] = var1 + var2;
                            break;
                        case NumberOperationTypes::SUBTRACT:
                            NumVariables[target] = var1 - var2;
                            break;
                        case NumberOperationTypes::MULTIPLY:
                            NumVariables[target] = var1 * var2;
                            break;
                        case NumberOperationTypes::DIVIDE:
                            NumVariables[target] = var1 / var2;
                            break;
                        case NumberOperationTypes::MODULO:
                            NumVariables[target] = var1 % var2;
                            break;
                        case NumberOperationTypes::POWER:
                            NumVariables[target] = pow(var1, var2);
                            break;
                    }
                }
                else {
                    assignVarToMapVariable(argument, target);
                }

                break;
            }

            case OperationTypes::DEF: {
                DefTypes type = defTypes[firstWord(arguments[i])];
                string argument = arguments[i];
                argument = removeFirstWord(argument);

                if (variableExists(firstWord(argument))) {
                    cerr << "Error: variable already exists!" << endl;
                    exit(9);
                }

                switch (type) {
                    case DefTypes::NUM:
                        NumVariables[firstWord(argument)] = getNumber(removeFirstWord(argument));
                        break;
                    case DefTypes::STR:
                        StrVariables[firstWord(argument)] = removeFirstWord(argument);
                        break;
                    case DefTypes::BOOL:
                        if (removeFirstWord(argument) == "true") {
                            BoolVariables[firstWord(argument)] = true;
                        }
                        if (removeFirstWord(argument) == "false") {
                            BoolVariables[firstWord(argument)] = false;
                        }
                        if (removeFirstWord(argument) != "true" && removeFirstWord(argument) != "false") {
                            cerr << "Error: bool variable gets a value other than bool!" << endl;
                            exit(5);
                        }
                        break;
                }
                break;
            }

            case OperationTypes::IF: {
                string argument = arguments[i];
                bool negation;

                if (conditionsContainsNot(firstWord(argument))) {
                    negation = true;
                    argument = removeFirstWord(argument);
                }
                else {
                    negation = false;
                }


                IfOperationTypes type = ifOperationTypes.at(firstWord(argument));
                argument = removeFirstWord(argument);

                long long var1, var2;

                var1 = getNumValueFromStringOrMap(firstWord(argument));
                argument = removeFirstWord(argument);
                var2 = getNumValueFromStringOrMap(firstWord(argument));

                bool expression = returnIfValue(var1, var2, type);

                if (negation) {
                    expression = !expression;
                }

                ifValues[i] = expression;

                if (!expression) {
                    i = findIntegerOfNestedEnd(i);
                }

                break;
            }

            case OperationTypes::ELSE: {
                if (instructions[i - 1] != OperationTypes::END) {
                    cerr << "Error: else without primary if!";
                    exit(10);
                }

                unsigned int integerOfParentIf = findPreviousIf(i);

                if (ifValues[integerOfParentIf] == true) {
                    i = findIntegerOfNestedEnd(i);
                    break;
                }
                break;
            }

            case OperationTypes::WHILE: {
                string argument = arguments[i];
                bool negation;

                if (conditionsContainsNot(firstWord(argument))) {
                    negation = true;
                    argument = removeFirstWord(argument);
                }
                else {
                    negation = false;
                }

                IfOperationTypes type = ifOperationTypes.at(firstWord(argument));
                argument = removeFirstWord(argument);

                long long var1, var2;

                var1 = getNumValueFromStringOrMap(firstWord(argument));
                argument = removeFirstWord(argument);
                var2 = getNumValueFromStringOrMap(firstWord(argument));

                bool expression = returnIfValue(var1, var2, type);

                if (negation) {
                    expression = !expression;
                }

                if (!expression) {
                    i = findIntegerOfNestedEnd(i);
                }
                break;
            }

            case OperationTypes::END: {
                if (endIsEndOfWhile(i)) {
                    i = findPreviousWhile(i);
                }
                break;
            }

            case OperationTypes::KILL: {
                // This is the easiest way to keep the program from closing
                char ch;
                cin >> ch;
                exit(0);
                break;
            }

            case OperationTypes::INPUT: {
                if (!variableExists(arguments[i])) {
                    cerr << "Error: variable does not exist!" << endl;
                    exit(10);
                }

                string value;
                cin >> value;

                assignVarToMapVariable(value, arguments[i]);

                break;
            }
        }
    }
    cout << "\n\nExecution complete.\n";
}

void selectPath() {
    const fs::path programPath = fs::current_path();
    fs::path path;

    cout << "Enter directory path (or type '0' to use the interpreter directory):\n";
    cin >> path;
    if (path == "0") {
        path = programPath;
    }
    cout << "Current directory: " << path << endl;

    vector<fs::path> files;
    for (const auto & entry : fs::directory_iterator(path)) {
        if (fs::is_regular_file(entry.status()) &&
            entry.path().extension() == ".ml"
        ) {
            files.push_back(entry.path());
        }
    }

    cout << "\nAvailable files:\n";
    for (int i = 0; i < files.size(); i++) {
        cout << "[" << i << "] " << files[i] << endl;
    }

    unsigned short select;
    cout << "\nSelect a program to run: \n";
    cin >> select;
    Path = files[select];
}

int main() {
    selectPath();
    loadProgram();
    formatProgramInput();
    translateInputIntoInstructions();
    executeInstructions();
    // This is the easiest way to keep the program from closing
    char ch;
    cin >> ch;
    return 0;
}
