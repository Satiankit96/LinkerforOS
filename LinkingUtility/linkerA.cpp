#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <map>
#include <cstdlib>
#include <stdio.h>
#include <string.h>


using namespace std;


//Constants Used accross file
const int MAX_MACHINE_SIZE = 512;
const int MAX_CHAR_SIZE  = 16;
const int MAX_LINE_COUNT = 16;
string word;
int tknCnt=0;

//Global Variables to be used by multiple readings and classes
string filename;
ifstream file;


//extra method to replace to_string
string integerToString(int x) {
    ostringstream convert;
    convert << x;
    return convert.str();
}
 
class Error {
    int add;
    int instrNum;
    int moduleNum;
    int code;
    string symbol;

public:
    void setAddress(int add) {
        this->add = add;
    }
    
    void setCode(int code) {
        this->code = code;
    }

    void setInstructionNumber(int instrNum) {
        this->instrNum = instrNum;
    }

    void setModuleNumber(int moduleNum) {
        this->moduleNum= moduleNum;
    }

    void setSymbol(string symbol) {
        this->symbol = symbol;
    }

    int getAddress() {
        return add;
    }

    int getInstructionNumber() {
        return instrNum;
    }

    int getModuleNumber() {
        return moduleNum;
    }

    string getErrorMessage() {
        //find message according to code
        switch(code) {
            case 8 : return "Error: Absolute address exceeds machine size; zero used";break;
            case 9 : return "Error: Relative address exceeds module size; zero used";break;
            case 6 : return "Error: External address exceeds length of uselist; treated as immediate";break;
            case 3 : return "Error: " + symbol + " is not defined; zero used";break;
            case 2 : return "Error: This variable is multiple times defined; first value used";break;
            case 10 : return "Error: Illegal immediate value; treated as 9999";break;
            case 11 : return "Error: Illegal opcode; treated as 9999";break;
            default: return "Error";
        }
    }
};


class Warning {
    int code;
    int moduleNum;
    int size;
    string symbol;
    int moduleSize;

public :
    void setCode(int code) {
        this->code = code;
    }

    void setModuleNumber(int moduleNum) {
        this->moduleNum = moduleNum;
    }

    void setSize(int size) {
        this->size = size;
    }
    
    void setSymbol(string symbol) {
        this->symbol = symbol;
    }

    void setModuleSize(int moduleSize) {
        this->moduleSize = moduleSize;
    }

    string getWarningMessage() {
        //send warning message according to code
        switch(code) {
            case 5 : return "Warning: Module " + integerToString(moduleNum+1) + ": " + symbol + " too big " + integerToString(size) + " (max="+integerToString(moduleSize)+") assume zero relative";break;
            case 7 : return "Warning: Module " + integerToString(moduleNum+1) + ": " + symbol + " appeared in the uselist but was not actually used";break;
            case 4 : return "Warning: Module " + integerToString(moduleNum+1) + ": " + symbol + " was defined but never used";break;
            default : return "Error";
        }
    }
};


int errLineNum=0;
int errLineOffset=0;


void setLineNumAndOffset(bool isNextToken) {
    string line, tkn;
    file.open(filename.c_str());
    
    //handle case where the file ended, was unable to get next token altogether
    if(isNextToken==true) {
        
        while(getline(file, line)) {
            errLineNum=errLineNum+1;
            errLineOffset = line.size();
        }
        errLineOffset=errLineOffset+1;
        return;
    }

    while(getline(file, line)) {
        errLineNum=errLineNum+1;
        stringstream linestream(line);
        errLineOffset = 0;
        while(getline(linestream, tkn,' ')) {
            if(!tkn.empty() && tkn != "\t" && tkn !=" ") {
			tknCnt--;
             } 
            if(tknCnt == 0) {
                errLineOffset = line.find(tkn);
                errLineOffset++;
                return;
            }
        }
    }
    file.close();
}

void __parseerror(int errcode, bool isNextToken) {
    file.close();
    setLineNumAndOffset(isNextToken);
    static char* errstr[] = {
        "NUM_EXPECTED", // Number expect, anything >= 2^30 is not a number either
        "SYM_EXPECTED", // Symbol Expected
        "ADDR_EXPECTED", // Addressing Expected which is A/E/I/R
        "SYM_TOO_LONG", // Symbol Name is too long
        "TOO_MANY_DEF_IN_MODULE", // > 16
        "TOO_MANY_USE_IN_MODULE", // > 16
        "TOO_MANY_INSTR" // total num_instr exceeds memory size (512)
    };
    printf("Parse Error line %d offset %d: %s\n", errLineNum, errLineOffset, errstr[errcode]);
}





//Base class for Symbol base
class Symbol {
    string name;
    int val;
    int absAdd;
    int moduleNum;
public :
    void setName(string name) {
        this->name = name;
    }

    void setValue(int val) {
        this->val = val;
    }

    void setAbsoluteAddress(int absAdd) {
        this->absAdd = absAdd;
    }

    void setModuleNumber(int moduleNum) {
        this->moduleNum= moduleNum;
    }

    string getName() const{
        return name;
    }

    int getValue() {
        return val;
    }

    int getAbsoluteAddress() {
        return absAdd;
    }

    int getModuleNumber() {
        return moduleNum;
    }
};


//Base class for Instruction Data in Instruction Set
class Instruction {
    char type;
    int add;
    int opcode;
    int operand;
    vector<Error> errorList;
public:
    void setType(char type) {
        this->type = type;
    }

    void setAddress(int add) {
        this->add = add;
    }

    void setOpcode(int opcode) {
        this->opcode = opcode;
    }

    void setOperand(int operand) {
        this->operand = operand;
    }

    void addToErrorList(Error e) {
        errorList.push_back(e);
    }

    int getAddress() {
        return add;
    }

    char getType() {
        return type;
    }

    vector<Error> getErrorList() {
        return errorList;
    }

};


//Class for containing moduke related data
class Module {
    int startAdd;
    int endAdd;
    vector<Symbol> defList;
    vector<string> useList;
    vector<Instruction> instrList;
    vector<Error> errList;
    vector<Warning> warningList;

public :
    void setStartAddress(int startAdd) {
        this->startAdd = startAdd;
    }

    void setEndAddress(int endAdd) {
        this->endAdd = endAdd;
    }

    void setDefinitonList(vector<Symbol> defList) {
        this->defList = defList;
    }

    void setUseList(vector<string> useList) {
        this->useList = useList;
    }

    void setInstructionList(vector<Instruction> instrList) {
        this->instrList = instrList;
    }

    void addDefinition(Symbol definition) {
        defList.push_back(definition);
    }

    void addInstruction(Instruction instruction) {
        instrList.push_back(instruction);
    }

    void addErrorList(Error e) {
        errList.push_back(e);
    }

    void addWarningToList(Warning w) {
        warningList.push_back(w);
    }
    
    vector<Symbol> getDefinitionList() {
        return defList;
    }

    vector<string> getUseList() {
        return useList;
    }

    vector<Instruction> getInstructionList() {
        return instrList;
    }

    int getInstructionSize() {
        return instrList.size();
    }

    vector<Error> getErrorList() {
        return errList;
    }

    vector<Warning> getWarningList() {
        return warningList;
    }
};




vector<pair<Symbol, int> > symbolTable;
vector<string> multipleSymbolsList;
vector<Module> moduleList;
vector<Symbol> defSymbolList;
vector<string> usedSymbol;
vector<Warning> globalWarningsList;


//function to update line number and offsets which is useful in parse errors
void incrementTokenCnt() {
    
    tknCnt=tknCnt+1;
}


// functions for checking

bool check(string token){
int i=0;
while(i < token.length()) {
        if(!isdigit(token.at(i))) {
            return false;
        }
      i++;
    }
    return true;
}

bool isInteger(string token) { 
bool ans=check(token);
return ans;
}

bool helperCorrectDef(string token){
    if(!isalpha(token.at(0))) {
        //current word is not correct so not next
        __parseerror(1, false);
        return false;
    }
    if(token.size() > MAX_CHAR_SIZE) {
        //current word is an issue, not next
        __parseerror(3, false);
        return false;
    }
    return true;
}

bool isCorrectDef(string token) {
bool ans1=helperCorrectDef(token);
return ans1;
}

bool instrHelper(string word){
   if(word.size() > 1) {
        return false;
    }
    char c = word.at(0);
    if(c=='R' || c=='E' || c=='I' || c=='A') {
        return true;
    }
    return false;
}

bool isInstructionType(string word) {
bool ansinstr=instrHelper(word);
return ansinstr;
 
}

bool isInVector(vector<string> checkVector, string value) {
    for(int i=0;i < checkVector.size();i++) {
        if(checkVector[i] == value) {
            return true;
        }
    }
    return false;
}


// contains all the functions for token conversion and readability


bool needNewLine = true;
int lineBuf[4096];

char *getToken() {
    char *tok;
    string currLine;
    while(needNewLine && !file.eof()) {
       getline(file, currLine);
       tok = strtok(&currLine[0], " \t\n");
       if(!tok) continue;
       needNewLine = false;
       return tok;
    }
    tok = strtok(NULL, " \t\n");
    if(!tok) {
        needNewLine = true;
        return getToken();
    }
    return tok;
}

vector<string> getTokenizedLine(string line) {
    vector<string> tokensList;
    char* ptr = &line[0];
    char* word;
    while ((word = strtok(ptr, " \t\n"))) {
        string k = word;
        tokensList.push_back(k);
        ptr = NULL;
    }
    return tokensList;
}

int readInt(string token) {
  int re=atoi(token.c_str());
    return re;
}

void printLineNum(int currentCount) {
    if(currentCount < 10) {
        cout<<"00"<<currentCount;
    } else if(currentCount >= 10 && currentCount < 100) {
        cout<<"0"<<currentCount;
    } else {
        cout<<currentCount;
    }
    return;
}

void printInstructionAddress(int instructionAddress) {
    if(instructionAddress < 10) {
        cout<<"000"<<instructionAddress;
    } else if(instructionAddress >= 10 && instructionAddress < 100) {
        cout<<"00"<<instructionAddress;
    } else if(instructionAddress >= 100 && instructionAddress < 1000){
        cout<<"0"<<instructionAddress;
    } else {
        cout<<instructionAddress;
    }
    return;
}

Module createModule(int startAddress) {
    Module module;
    module.setStartAddress(startAddress);
    return module;
}

int calculateAbsAddress(int baseAddress, string relativeAddress) {
    return baseAddress + atoi(relativeAddress.c_str());
}

int helperAdd(string symbolName){
    int i=0;
    while(i<symbolTable.size()){
	if(symbolTable[i].first.getName() == symbolName) {
        	return symbolTable[i].second;
        }
    i++;
    }
    return -1;
}

int findAddressInSymbolTable(string symbolName) {
int findans=helperAdd(symbolName);
return findans;
}

bool inUsedSymbolList(string symbol) {
    for(int i=0;i<usedSymbol.size();i++) {
        if(usedSymbol[i] == symbol) {
            return true;
        }
    }
    return false;
}


bool presentInDefSymbolList(string symbolName) {
    for(int i=0; i< defSymbolList.size(); i++) {
        if(defSymbolList[i].getName() == symbolName) {
            return true;
        }
    }
    return false;
} 

void checkUseOfSymbolsForInstructionSet(vector<string> useList, vector<string> usedSymbol, int moduleIterator, Module &module) {
    for(int i = 0; i < useList.size(); i++) {
        if(!isInVector(usedSymbol, useList[i])) {
            Warning w;
            w.setCode(7);
            w.setModuleNumber(moduleIterator);
            w.setSymbol(useList[i]);
            module.addWarningToList(w);
        }
    }
}



void readDefinitionsForPass1(int defNums, int baseModuleAddress, int moduleIterator) {
    int tknCnt = 1;
    string preTkn;
    while(tknCnt <= defNums*2) {
        if(!(file>>word)) {
            if(tknCnt%2) {
                //case where there is no def name is given
                __parseerror(1, true);
                exit(1);
            } 
       else {
                //case where address of def name is not given
                __parseerror(0, true);
                exit(1);
            }
        }
        string tkn = word;
        incrementTokenCnt();
        if(tknCnt%2) {
            if(!isCorrectDef(tkn)) {
                //case where def is not according to specifications
                //errors are handled in correct def func
                exit(1);
            }

            if(findAddressInSymbolTable(tkn) != -1) {
                //case where symbol already present
                multipleSymbolsList.push_back(tkn);
                tkn = "multiple";
            }
            preTkn = tkn;
        } else {
            if(!isInteger(tkn)) {
                //case where the address is not integer
                __parseerror(0, false);
                exit(1);
            }
            //add to symbol table
            if(preTkn != "multiple") {
                Symbol symbol;
                symbol.setName(preTkn);
                symbol.setModuleNumber(moduleIterator);
                symbol.setAbsoluteAddress(calculateAbsAddress(baseModuleAddress, tkn));
                symbol.setValue(atoi(tkn.c_str()));
                symbolTable.push_back(pair<Symbol, int>(symbol, calculateAbsAddress(baseModuleAddress, tkn)));
            }
        }
        tknCnt++;  
    }
}


void readUsageListForPass1(int usageCount) {
    int currUseNum = 1;
    while(currUseNum <= usageCount) {
        if(!(file>>word)) {
            //case of usage list not equal to usage count
            // next word not found, hence true
            __parseerror(1, true);
            exit(1);
        }
        incrementTokenCnt();
        if(!isCorrectDef(word)) {
            //error parsing handled in function
            exit(1);
        }
        currUseNum++;
    }
}

void readInstructionListForPass1(int instructionCount) {
    int currInstrNum = 1, opcode, address;
    char instrType;

    while(currInstrNum <= instructionCount*2) {
        if(!(file>>word)) {
            if(currInstrNum%2==1) {
                //expected , but wasn't hence next token
                __parseerror(2, true);
                exit(1);
            } 
         else {
                //expected, but wasn't hence next token
                __parseerror(0, true);
                exit(1);
            }
        }
        incrementTokenCnt();
        //case of R,E,I,A
        if(currInstrNum%2==1) {
            if(!isInstructionType(word)) {
                //not the next word, this word failed
                __parseerror(2, false);
                exit(1);
            }
            instrType = word.at(0);
        } 
        else {
            //case of instruction
            if(!isInteger(word)) {
                //this current instruction failed
                __parseerror(0, false);
                exit(1);
            }
            int instrValue = atoi(word.c_str());
            opcode = instrValue/1000; 
            address =instrValue%1000;
            //checks on opcode and address in Pass 2 

        }
        currInstrNum++;
    }
}


//PASS 2 definitions , definitions usage and instructions


vector<Symbol> readDefinitionsForPass2(int definitionCount, int moduleNumber) {
    vector<Symbol> symbolList;
    string currSymName;
    int currTkn = 1;
    while((currTkn <= 2*definitionCount) && (file>>word)) {
        if(currTkn%2==1) {
            currSymName = word;
            if(findAddressInSymbolTable(currSymName) == -1) {
                //if symbol not present in case of error
                currSymName = "ERROR";
            }
        } else {
            if(currSymName!="ERROR") {
                Symbol symbol;
                symbol.setName(currSymName);
                symbol.setAbsoluteAddress(findAddressInSymbolTable(currSymName));
                symbol.setModuleNumber(moduleNumber);
                symbolList.push_back(symbol);
                if(!presentInDefSymbolList(symbol.getName())) {
                    defSymbolList.push_back(symbol);
                } 
            }
        }
        currTkn++;
    }
    return symbolList;
}


vector<string> readUsageListForPass2(int usageCount) {
    int currUseNum = 1;
    vector<string> useList;
    string word;
    while((currUseNum <= usageCount) && (file>>word)) {
        useList.push_back(word);
        currUseNum++;
    }
    return useList;
}


vector<Instruction> readInstructionListForPass2(int instructionCount, int currentModAddress, vector<string>useList, int moduleIterator, Module &module) {
    

    char type;
    string word;
    int opcode=1;
    int add=1; 
    int currInstrCnt = 1;
    vector<Instruction> instructionList;

    while((currInstrCnt <= 2*instructionCount) && (file>>word)) {
        if(currInstrCnt%2==1) {
            //case of R,E,I,A
            type = word.at(0);
        } 
        else {
            //case of instruction address
            Instruction in;
            in.setType(type);
            int defAdd = atoi(word.c_str());
            opcode = defAdd/1000;
            if(type == 'I') {
                if(defAdd > 9999) {
                    //error case of I going over 9999
                    Error e;
                    e.setModuleNumber(moduleIterator);
                    e.setCode(10);
                    module.addErrorList(e);
                    in.addToErrorList(e);
                    defAdd = 9999;
                }
                add = defAdd;
            } 
            else if(opcode >= 10) {
                //illegal opcode first check
                Error e;
                e.setCode(11);
                e.setModuleNumber(moduleIterator);
                module.addErrorList(e);
                in.addToErrorList(e);
                add = 9999;
            } 
            else if(type== 'R') {
                //case of R going over module size
                if(defAdd%1000 > instructionCount) {
                    Error e;
                    e.setCode(9);
                    module.addErrorList(e);
                    in.addToErrorList(e);
                    defAdd = opcode*1000 + 0;
                }
                add = defAdd + currentModAddress;
            } 
            else if(type == 'A') {
                if(defAdd%1000 > MAX_MACHINE_SIZE) {
                    //error in case of A overshooting max size 
                    Error e;
                    e.setModuleNumber(moduleIterator);
                    e.setCode(8);
                    module.addErrorList(e);
                    in.addToErrorList(e);
                    defAdd = (defAdd/1000) * 1000;
                }
                add = defAdd;
            } 
            else {
                //case of 'E' or external, based on use list
                int index = defAdd%1000;
                if(index > useList.size()-1) {
                    Error e;
                    e.setCode(6);
                    module.addErrorList(e);
                    in.addToErrorList(e);
                    add = defAdd;
                } 
             else {
                    int externalAdd = findAddressInSymbolTable(useList[index]);
                    if(externalAdd == -1) {
                        //unable to find symbol
                        Error e;
                        e.setInstructionNumber(currInstrCnt);
                        e.setModuleNumber(moduleIterator);
                        e.setSymbol(useList[index]);
                        e.setCode(3);
                        module.addErrorList(e);
                        in.addToErrorList(e);
                        externalAdd = 0;                        
                    }
                    add = (defAdd/1000)*1000 + externalAdd; 
                    
                    //push symbols as used for later check
                    usedSymbol.push_back(useList[index]);
                }
            }
            in.setOpcode(defAdd/1000);
            in.setType(type);
            in.setAddress(add);
            instructionList.push_back(in);
        }
        currInstrCnt++;
    }
    checkUseOfSymbolsForInstructionSet(useList, usedSymbol, moduleIterator, module);
    return instructionList;
}


// writing base parsing logic for PASS 1

int readFilePass1() {
    file.open(filename.c_str());
    string tkn;
    string currLine;
    //start with module address with 0
    int currModAdd = 0, moduleItr= 0;
    if (file.is_open()) {
        while(file>>word) {

            // get definitions with (S,R) values
            incrementTokenCnt();
            if(!isInteger(word)) {
                //issue with current word not being integer
                __parseerror(0, false);
                exit(1);
            }
            int defNums = readInt(word);
            if(defNums > MAX_CHAR_SIZE) {
                //issue with current word going beyond size
                __parseerror(4, false);
                exit(1);
            }

            //check defintions and errors in Pass 1
            readDefinitionsForPass1(defNums, currModAdd, moduleItr);

            
            //get usages of S values
            if(!(file>>word)) {
                //unable to find S value
                __parseerror(0, true);
                exit(1);
            }
            incrementTokenCnt();
            if(!isInteger(word)) {
                //current token is not a word
                __parseerror(0, false);
                exit(1);
            }
            int useCnt = readInt(word);
            if(useCnt > MAX_LINE_COUNT) {
                //too may uses parse error, should declare for same point
                __parseerror(5, false);
                exit(1);
            }

            //check the individual usage list and count
            readUsageListForPass1(useCnt); 


            //get instruction set for pass 1
            if(!(file>>word)) {
                //instructions not found, looking for next token
                __parseerror(0, true);
                exit(1);
            }
            incrementTokenCnt();
            if(!isInteger(word)) {
                //word not found in current
                __parseerror(0, false);
                exit(1);
            }
            int instrCnt = readInt(word);
            if(instrCnt + currModAdd > MAX_MACHINE_SIZE) {
                //current instruction count an issue
                __parseerror(6, false);
                exit(1);
            }
            readInstructionListForPass1(instrCnt);

            currModAdd += instrCnt;

            //add the warnings post pass 1 (rule 5)
            //length equivalent to instructionCount
            for(int i=0; i< symbolTable.size(); i++) {
                int relValue = symbolTable[i].first.getValue();
                if(symbolTable[i].first.getModuleNumber()==moduleItr &&  relValue> instrCnt-1) {
                    //case where beyond size
                    Warning w;
                    w.setCode(5);
                    w.setSymbol(symbolTable[i].first.getName());
                    w.setModuleNumber(moduleItr);
                    w.setSize(relValue);
                    w.setModuleSize(instrCnt-1);
                    globalWarningsList.push_back(w);
                    //replace with zero relative
                    symbolTable[i].first.setValue(0);
                    symbolTable[i].first.setAbsoluteAddress(currModAdd-instrCnt);
                    symbolTable[i].second = currModAdd-instrCnt;
                }
            }
            moduleItr++;
        }
    }
    file.close();
    return 1;
}

//write base logic for parsing file for PASS 2

int readFilePass2() {
    file.open(filename.c_str());
    string currLine;
    //start with module address as 0
    int currModAdd = 0, moduleItr = 0;


    if(file.is_open()) {
        while(file>>word) {
            Module module;

            //read definitions
            
            int defCnt = atoi(word.c_str());
            module.setDefinitonList(readDefinitionsForPass2(defCnt, moduleItr));

            //read and store usages
            file>>word;
            int usageCount = atoi(word.c_str());
            module.setUseList(readUsageListForPass2(usageCount));

            //read and relate addressess for instructions
            file>>word;
            int instrCnt = atoi(word.c_str());
            module.setInstructionList(readInstructionListForPass2(instrCnt, currModAdd, module.getUseList(), moduleItr, module));
            
            //add module to final list for bookkeeping
            moduleList.push_back(module);
            currModAdd=currModAdd+module.getInstructionSize();
            moduleItr++;
        }
    }


    //print Warnings list global rule 5 ?
    for(int j=0; j< globalWarningsList.size(); j++) {
        cout<<globalWarningsList[j].getWarningMessage()<<endl;
    }
    cout<<endl;


    //print the Symbol Table
    cout<<"Symbol Table" << endl;
    for(int i = 0; i < symbolTable.size(); i++) {
        cout<<symbolTable[i].first.getName()<<"="<<symbolTable[i].second;
        for(int j = 0; j < multipleSymbolsList.size(); j++) {
            if(multipleSymbolsList[j] == symbolTable[i].first.getName()) {
               //print message string for warning/error
                cout<<" Error: This variable is multiple times defined; first value used";
            }
        }
        cout<<endl;
    }
    cout<<endl;
    
    //now print the modules accordingly 
    cout<<"Memory Map"<<endl;
    int cnt = 0;
    for(int i = 0; i < moduleList.size(); i++) {
        Module currModule = moduleList[i];
        for(int j=0; j < currModule.getInstructionSize(); j++) {
            vector<Instruction> instructionList = currModule.getInstructionList();
            printLineNum(cnt);
            cout<<": ";
            printInstructionAddress(instructionList[j].getAddress());
            if(instructionList[j].getErrorList().size() > 0) {
                cout<<" " << instructionList[j].getErrorList()[0].getErrorMessage();
            }
            cout<<endl;
            cnt++;
        }
        //print end of module warnings rule 7
        for(int k=0; k < currModule.getWarningList().size(); k++) {
            cout << currModule.getWarningList()[k].getWarningMessage() << endl; 
        }
    }
    cout<<endl;

    //print end warnings rule 4
    for(int i=0; i<defSymbolList.size(); i++) {
        if(!inUsedSymbolList(defSymbolList[i].getName())) {
            Warning w;
            w.setCode(4);
            w.setSymbol(defSymbolList[i].getName());
            w.setModuleNumber(defSymbolList[i].getModuleNumber());
            cout << w.getWarningMessage() << endl;
        }
    }
    
    file.close();
    return 1;
}


int main(int argc, char* argv[]) {
    filename = argv[1];
    //filename = "example.txt";
    readFilePass1();
    readFilePass2();
    
    //clear and close out variables
    symbolTable.clear();
    multipleSymbolsList.clear();

    return 0;
}
