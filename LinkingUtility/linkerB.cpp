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

const int MAX_MACHINE_SIZE = 512;
const int MAX_LINE_COUNT = 16;
const int MAX_CHAR_SIZE  = 16;


string word;
int tknCnt=0;

string filename;
ifstream file;

string integerToString(int x) {
    ostringstream convert;
    convert << x;
    return convert.str();
} 
class Warning {
    int size;
    int moduleNum;
    int code;
    int moduleSize;
    string symbol;

public :

    void setSize(int size) {
        this->size = size;
    }
    void setModuleNumber(int moduleNum) {
        this->moduleNum = moduleNum;
    }
    void setCode(int code) {
        this->code = code;
    }
    void setModuleSize(int moduleSize) {
        this->moduleSize = moduleSize;
    }
    void setSymbol(string symbol) {
        this->symbol = symbol;
    }


    string getWarningMessage() {
        switch(code) {
            case 4 : return "Warning: Module " + integerToString(moduleNum+1) + ": " + symbol + " nEVER dEFINED OR uSED";break;
            case 7 : return "Warning: Module " + integerToString(moduleNum+1) + ": " + symbol + " appeared but not used" ;break;
            case 5 : return "Warning: Module " + integerToString(moduleNum+1) + ": " + symbol + " big " + integerToString(size) + " (max="+integerToString(moduleSize)+") assume zero ";break;

            default : return "Error";
        }
    }
};
class Symbol {
    int absAdd; 
    int val;
    int moduleNum;
    string name;
public :

    void setAbsoluteAddress(int absAdd) {
        this->absAdd = absAdd;
    }
    void setValue(int val) {
        this->val = val;
    }
    void setModuleNumber(int moduleNum) {
        this->moduleNum= moduleNum;
    }
    void setName(string name) {
        this->name = name;
    }

    int getAbsoluteAddress() {
        return absAdd;
    }
    int getValue() {
        return val;
    }
    int getModuleNumber() {
        return moduleNum;
    }
    string getName() const{
        return name;
    }

};
class Error {
    int add;
    int code;
    int instrNum;
    int moduleNum;
    string symbol;

public:

    void setCode(int code) {
        this->code = code;
    }

    void setAddress(int add) {
        this->add = add;
    }
        
    void setModuleNumber(int moduleNum) {
        this->moduleNum= moduleNum;
    }

    void setInstructionNumber(int instrNum) {
        this->instrNum = instrNum;
    }

    void setSymbol(string symbol) {
        this->symbol = symbol;
    }

    int getAddress() {
        return add;
    }

    int getModuleNumber() {
        return moduleNum;
    }

    int getInstructionNumber() {
        return instrNum;
    }

    string getErrorMessage() {
        switch(code) {
            case 2 : return "Error: This variable is multiple times defined; first value used";break;
            case 3 : return "Error: " + symbol + " is not defined; NONE used";break;
            case 9 : return "Error: Relative address exceeds size;  used";break;
            case 8 : return "Error: Absolute address exceeds size; NONE used";break;
            case 6 : return "Error: External address exceeds length ; treated as immediate";break;
            case 11 : return "Error: Illegal opcode; treated as 9999";break;
            case 10 : return "Error: Illegal immediate value; treated as 9999";break;

            default: return "Error STATE";
        }
    }
};


int errLineNum=0;
int errLineOffset=0;


void setLineNumAndOffset(bool isNextToken) {
    string line, tkn;
    file.open(filename.c_str());
    
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
        "TOO_MANY_DEF_IN_MODULE",
        "TOO_MANY_USE_IN_MODULE", 
        "NUM_EXPECTED", 
        "SYM_EXPECTED", 
        "ADDR_EXPECTED",
        "TOO_MANY_INSTR" , 
        "SYM_TOO_LONG"
    };
    printf("Parse Error line %d offset %d: %s\n", errLineNum, errLineOffset, errstr[errcode]);
}

class Instruction {
    int operand;
    int opcode;
    int add;
    char type;
    
    vector<Error> errorList;
public:

    void setOperand(int operand) {
        this->operand = operand;
    }
    void setOpcode(int opcode) {
        this->opcode = opcode;
    }
    void setAddress(int add) {
        this->add = add;
    }    
    void setType(char type) {
        this->type = type;
    }

    void addToErrorList(Error e) {
        errorList.push_back(e);
    }

    char getType() {
        return type;
    }
    int getAddress() {
        return add;
    }

    vector<Error> getErrorList() {
        return errorList;
    }

};

class Module {
    int endAdd;
    int startAdd;
    vector<Warning> warningList;
    vector<Error> errList;
    vector<Instruction> instrList;  
    vector<string> useList;
    vector<Symbol> defList;
  
public :

    void setEndAddress(int endAdd) {
        this->endAdd = endAdd;
    }

    void addDefinition(Symbol definition) {
        defList.push_back(definition);
    }

    void addInstruction(Instruction instruction) {
        instrList.push_back(instruction);
    }

    void setUseList(vector<string> useList) {
        this->useList = useList;
    }
    void setDefinitonList(vector<Symbol> defList) {
        this->defList = defList;
    }

    void setInstructionList(vector<Instruction> instrList) {
        this->instrList = instrList;
    }
    void setStartAddress(int startAdd) {
        this->startAdd = startAdd;
    }
    void addWarningToList(Warning w) {
        warningList.push_back(w);
    }

    void addErrorList(Error e) {
        errList.push_back(e);
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

vector<Module> moduleList;
vector<Symbol> defSymbolList;
vector<string> multipleSymbolsList;
vector<string> usedSymbol;
vector<pair<Symbol, int> > symbolTable;
vector<Warning> globalWarningsList;

void incrementTokenCnt() {
    
    tknCnt=tknCnt+1;
}

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
        __parseerror(1, false);
        return false;
    }
    if(token.size() > MAX_CHAR_SIZE) {
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
    int i = 0;
    while (i < checkVector.size())
    {
        if(checkVector[i] == value) {
            return true;
        }
        i++;
    }
    return false;
}

int lineBuf[4096];
bool  newLine = true;

char *getToken() {
    char *t;
    string currentLine;
    while(newLine && !file.eof()) {
       getline(file, currentLine);
       t = strtok(&currentLine[0], " \t\n");
       if(!t) continue;
    
       newLine = false;
       return t;
       
    }
    t = strtok(NULL, " \t\n");
    if(!t) {
        newLine = true;
        return getToken();
    }
    return t;
}

vector<string> getTokenizedLine(string line) {
    vector<string> tknsList;
    char* word;
    char* p = &line[0];

    while ((word = strtok(p, " \t\n"))) {
        string z = word;
        tknsList.push_back(z);
        p = NULL;
    }
    return tknsList;
}

int readInt(string token) {
  int re=atoi(token.c_str());
    return re;
}

void printLineNum(int currCount) {
    if(currCount < 10) {
        cout<<"00"<<currCount;
    } else if(currCount >= 10 && currCount < 100) {
        cout<<"0"<<currCount;
    } else {
        cout<<currCount;
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
    int ans = baseAddress + atoi(relativeAddress.c_str());
    return ans;
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

bool presentInDefSymbolList(string symbolName) {
    int i = 0;
    while(i< defSymbolList.size())
    {
        if(defSymbolList[i].getName() == symbolName) {
            return true;
        }
         i++;
    }
    return false;
}
void readDefinitionsForPass1(int defNums, int baseModuleAddress, int moduleIterator) {
    int tknCnt = 1;
    string preTkn;
    while(tknCnt <= defNums*2) {
        if(!(file>>word)) {
            if(tknCnt%2) {
                __parseerror(1, true);
                exit(1);
            } 
       else {
                __parseerror(0, true);
                exit(1);
            }
        }
        string tkn = word;
        incrementTokenCnt();
        if(tknCnt%2) {
            if(!isCorrectDef(tkn)) {
                exit(1);
            }

            if(findAddressInSymbolTable(tkn) != -1) {
                multipleSymbolsList.push_back(tkn);
                tkn = "multiple vals";
            }
            preTkn = tkn;
        } else {
            if(!isInteger(tkn)) {
                __parseerror(0, false);
                exit(1);
            }
            if(preTkn != "multiple vals") {
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
void checkUseOfSymbolsForInstructionSet(vector<string> useList, vector<string> usedSymbol, int moduleIterator, Module &module) {
    
    int i = 0;
    while (i < useList.size())
    
    {
        if(!isInVector(usedSymbol, useList[i])) {
            Warning w;
            w.setCode(7);
            w.setModuleNumber(moduleIterator);
            w.setSymbol(useList[i]);
            module.addWarningToList(w);
        }
        i++;
    }
}
void helperReadFUnction(int instructionCount){
    char instrType;
    int currInstrNum = 1;
    int address;
    int opcode;

    while(currInstrNum <= instructionCount*2) {
        if(!(file>>word)) {
            if(currInstrNum%2==1) {
                __parseerror(2, true);
                exit(1);
            } 
         else {
                __parseerror(0, true);
                exit(1);
            }
        }
        incrementTokenCnt();
        if(currInstrNum%2==1) {
            if(!isInstructionType(word)) {
                __parseerror(2, false);
                exit(1);
            }
            instrType = word.at(0);
        } 
        else {
            if(!isInteger(word)) {
                __parseerror(0, false);
                exit(1);
            }
            int instrValue = atoi(word.c_str());
            opcode = instrValue/1000; 
            address =instrValue%1000;
        }
        currInstrNum++;
    }

}
void readInstructionListForPass1(int instructionCount) {
    helperReadFUnction(instructionCount);

}
bool inUsedSymbolList(string symbol) {
    int i =0;
    while (i<usedSymbol.size())
    {
        if(usedSymbol[i] == symbol) {
            return true;
        }
        i++;
    }
    return false;
}

void readUsageListForPass1(int usageCount) {
    int currUseNum = 1;
    while(currUseNum <= usageCount) {
        if(!(file>>word)) {
            __parseerror(1, true);
            exit(1);
        }
        incrementTokenCnt();
        if(!isCorrectDef(word)) {
            exit(1);
        }
        currUseNum = currUseNum+1;
    }
}
vector<string> readUsageListForPass2(int usageCount) {
    int currUseNum = 1;
    vector<string> useList;
    string word;
    while((file>>word) && (currUseNum <= usageCount)) {
        useList.push_back(word);
        currUseNum++;
    }
    return useList;
}
vector<Symbol> readDefinitionsForPass2(int definitionCount, int moduleNumber) {
    vector<Symbol> symbolList;
    string currSymName;
    int currTkn = 1;
    while((file>>word) && (currTkn <= 2*definitionCount)) {
        if(currTkn%2==1) {
            currSymName = word;
            if(findAddressInSymbolTable(currSymName) == -1) {
                currSymName = "ERROR Found";
            }
        } else {
            if(currSymName!="ERROR Found") {
                Symbol symbol;
                symbolList.push_back(symbol);
                symbol.setModuleNumber(moduleNumber);
                symbol.setAbsoluteAddress(findAddressInSymbolTable(currSymName));
                symbol.setName(currSymName);

                if(!presentInDefSymbolList(symbol.getName())) {
                    defSymbolList.push_back(symbol);
                } 
            }
        }
        currTkn++;
    }
    return symbolList;
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
            type = word.at(0);
        } 
        else {
            Instruction in;
            in.setType(type);
            int defAdd = atoi(word.c_str());
            opcode = defAdd/1000;
            if(opcode >= 10) {
                Error e;
                
                e.setModuleNumber(moduleIterator);
                in.addToErrorList(e);
                module.addErrorList(e);
                e.setCode(11);
                add = 9999;
            }  

            else if(type == 'I') {
                if(defAdd > 9999) {
                    Error e;
                    module.addErrorList(e);
                    e.setCode(10);
                    e.setModuleNumber(moduleIterator);
                    in.addToErrorList(e);
                    

                    defAdd = 9999;
                }
                add = defAdd;
            }           
            else if(type== 'R') {
                if(defAdd%1000 > instructionCount) {
                    Error e;
                    
                    in.addToErrorList(e);
                    module.addErrorList(e);
                    e.setCode(9);
                    
                    defAdd = opcode*1000 + 0;
                }
                add = defAdd + currentModAddress;
            }
          
            else if(type == 'A') {
                if(defAdd%1000 > MAX_MACHINE_SIZE) {
                    Error e;
                    module.addErrorList(e);
                    in.addToErrorList(e);
                    e.setCode(8);
                    e.setModuleNumber(moduleIterator);

                    defAdd = (defAdd/1000) * 1000;
                }
                add = defAdd;
            } 
            else {
                int idx = defAdd%1000;
                if(idx > useList.size()-1) {
                    Error e;
                    e.setCode(6);
                    module.addErrorList(e);
                    in.addToErrorList(e);
                    add = defAdd;
                } 
             else {
                    int externalAdd = findAddressInSymbolTable(useList[idx]);
                    if(externalAdd == -1) {
                        Error e;
                        e.setCode(3);
                        module.addErrorList(e);
                        in.addToErrorList(e);
                        e.setInstructionNumber(currInstrCnt);
                        e.setSymbol(useList[idx]);
                        e.setModuleNumber(moduleIterator);

                        externalAdd = 0;                        
                    }
                    add = (defAdd/1000)*1000 + externalAdd; 
                    
                    usedSymbol.push_back(useList[idx]);
                }
            }
            in.setOpcode(defAdd/1000);
            in.setAddress(add);
            in.setType(type);
            instructionList.push_back(in);
        }
        currInstrCnt++;
    }
    checkUseOfSymbolsForInstructionSet(useList, usedSymbol, moduleIterator, module);
    return instructionList;
}

int readFilePass1() {
    file.open(filename.c_str());
    string currLine;
    int currModAdd = 0;
    string tkn;
    int moduleItr= 0;
    if (file.is_open()) {
        while(file>>word) {

            incrementTokenCnt();
            if(!isInteger(word)) {
                __parseerror(0, false);
                exit(1);
            }
            int defNums = readInt(word);
            if(defNums > MAX_CHAR_SIZE) {
                __parseerror(4, false);
                exit(1);
            }

            readDefinitionsForPass1(defNums, currModAdd, moduleItr);

                        if(!(file>>word)) {
                __parseerror(0, true);
                exit(1);
            }
            incrementTokenCnt();
            if(!isInteger(word)) {
                __parseerror(0, false);
                exit(1);
            }
            int useCnt = readInt(word);
            if(useCnt > MAX_LINE_COUNT) {
                __parseerror(5, false);
                exit(1);
            }
            readUsageListForPass1(useCnt); 

            if(!(file>>word)) {
                __parseerror(0, true);
                exit(1);
            }
            incrementTokenCnt();
            if(!isInteger(word)) {
                __parseerror(0, false);
                exit(1);
            }
            int instrCnt = readInt(word);
            if(instrCnt + currModAdd > MAX_MACHINE_SIZE) {
                __parseerror(6, false);
                exit(1);
            }
            readInstructionListForPass1(instrCnt);

            currModAdd = currModAdd + instrCnt;
            int i =0; 
            while (i< symbolTable.size())

            {
                int relValue = symbolTable[i].first.getValue();
                if(symbolTable[i].first.getModuleNumber()==moduleItr &&  relValue> instrCnt-1) {
                    Warning w;
                    w.setCode(5);
                    w.setSymbol(symbolTable[i].first.getName());
                    w.setSize(relValue);
                    w.setModuleNumber(moduleItr);
                    w.setModuleSize(instrCnt-1);
                    globalWarningsList.push_back(w);
                    symbolTable[i].first.setValue(0);
                    symbolTable[i].first.setAbsoluteAddress(currModAdd-instrCnt);
                    symbolTable[i].second = currModAdd-instrCnt;
                }
                i++;
            }
            moduleItr++;
        }
    }
    file.close();
    return 1;
}
int readFilePass2() {
    file.open(filename.c_str());
    
    int moduleItr = 0;
    int currModAdd = 0;
    string currLine;
    
    if(file.is_open()) {
        while(file>>word) {
            Module module;            
            int defCnt = atoi(word.c_str());
            module.setDefinitonList(readDefinitionsForPass2(defCnt, moduleItr));

            file>>word;
            int usageCount = atoi(word.c_str());
            module.setUseList(readUsageListForPass2(usageCount));

            file>>word;
            int instrCnt = atoi(word.c_str());
            module.setInstructionList(readInstructionListForPass2(instrCnt, currModAdd, module.getUseList(), moduleItr, module));
            
            moduleList.push_back(module);
            currModAdd=currModAdd+module.getInstructionSize();
            moduleItr++;
        }
    }
    int j=0;
    while (j< globalWarningsList.size())
    {
        cout<<globalWarningsList[j].getWarningMessage()<<endl;
        j++;
    }
    cout<<endl;

    cout<<"Symbol table" << endl;
    for(int i = 0; i < symbolTable.size(); i++) {
        cout<<symbolTable[i].first.getName()<<"="<<symbolTable[i].second;
        for(int j = 0; j < multipleSymbolsList.size(); j++) {
            if(multipleSymbolsList[j] == symbolTable[i].first.getName()) {
                cout<<" Error: This variable is multiple times defined; first value used";
            }
        }
        cout<<endl;
    }
    cout<<endl;
    
        cout<<"Memory map"<<endl;
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
        int k = 0;
        while(currModule.getWarningList().size())
        {
            cout << currModule.getWarningList()[k].getWarningMessage() << endl; 
            k++;
        }
    }
    cout<<endl;

    int i =0;
    while(i<defSymbolList.size())

    {
        if(!inUsedSymbolList(defSymbolList[i].getName())) {
            Warning w;
            w.setCode(4);
            w.setSymbol(defSymbolList[i].getName());
            w.setModuleNumber(defSymbolList[i].getModuleNumber());
            cout << w.getWarningMessage() << endl;
        }
        i++;
    }
    
    file.close();
    return 1;
}
int main(int argc, char* argv[]) {
    filename = argv[1];
    readFilePass1();
    readFilePass2();
    
    symbolTable.clear();
    multipleSymbolsList.clear();

    return 0;
}