#include <iostream>
#include <cstdio>
#include <cctype>
#include <fstream>
#include <cstring>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

ifstream  my_file;
char  *file;
char  line[1024];
char *token;
char *delim;
int  curr_offset =0;  // denotes the current offset
int next_offset=0;
int  line_reset_count=1;  //condition for gettoken
int line_no =0;       //denotes the line number
int  memory_srno=0;
int numSymbols=0;

 // necessary items to print  symbol table
typedef struct SymbolTable{
    string name;
    int value;           // address value
    int relative;       // keeping track of relative address when warning 1 (rule 5 is encountered)(still not used)
    string error;
    int used;           // check if symbol is used in any module for warning 3 (rule 4)
    int module;
    struct SymbolTable *next;

} SymbolTable;

typedef struct modules
{
    int base_add;
    int length;
    int mod_num;
} modules;

SymbolTable symbolTable[512];
SymbolTable sym_details;
struct modules *curr_modules=NULL;


// contains the used symbols in current module
vector<string> usedSymbols;

// whether used symbols in current module are accessed in instructions
vector<int> usedSymbolsCheck;
// denotes the current module being parsed and size of the module i.e. no of instructions
int curr_mod_mo = 0;
int curr_mod_size = 0;

vector<SymbolTable> sym_t;
vector<SymbolTable> use_list;

void __parseerror(int errcode)
{
    static string errorStrings[] = {
        "NUM_EXPECTED",           // Number expect
        "SYM_EXPECTED",           // Symbol Expected
        "ADDR_EXPECTED",          // Addressing Expected which is A/E/I/R
        "SYM_TOO_LONG",           // Symbol Name is too long
        "TOO_MANY_DEF_IN_MODULE", // > 16
        "TOO_MANY_USE_IN_MODULE", // > 16
        "TOO_MANY_INSTR",         // total num_instr exceeds memory size  (512)
    };
    cout << "Parse Error line " << line_no << " offset " << curr_offset+1 << ": " << errorStrings[errcode] << endl;
}


// Errors list
string __error__(int errcode)
{
    static string errorStrings[] = {
        "Error: Absolute address exceeds machine size; zero used",
        "Error: Relative address exceeds module size; zero used",
        "Error: External address exceeds length of uselist; treated as immediate",
        "Error: This variable is multiple times defined; first value used",
        "Error: Illegal immediate value; treated as 9999",
        "Error: Illegal opcode; treated as 9999",
    };
    return errorStrings[errcode];
}

string getToken()
{
    while (!token)
    {
        my_file.getline(line, 1024);
        if (my_file.eof())
        {
            curr_offset = next_offset;
            return "-EOF-";         // return special token to denote end of file
        }
        line_no++;
        next_offset = strlen(line);
        token = strtok(line, " \t\n");
    }
    curr_offset = token - line;
    string x(token);
    token = strtok(NULL, " \t\n");
    return x;
}
/*string getToken()
    
    {
        
        if(line_reset_count){
             
            
            if (my_file.eof()) {
                curr_offset = next_offset;
                string rt_value= "-1. We have recieved end of file";
                
                return  rt_value;
            }
            
            //while(!my_fileW.eof()){
                
                my_file.getline(line,1024);
                if(my_file.eof()){
                    curr_offset = next_offset;
                    string rt_value= "1. We have recieved end of file";
                
                    return  rt_value;

                }
                line_no++;
                line_reset_count=0;
                next_offset=1;
                token=strtok(line," \t\n");
                
                
            //}
           

            
        }
        if(token!=NULL){
           // cout << "entered token" << "\n";
            curr_offset = next_offset;
        
            next_offset += strlen(token)+1;
            char  *tok = token;
            token = strtok(NULL," \t\n");
            //cout << tok << "\n";
            return tok;
            

        }else{
             
            line_reset_count=1;
            return getToken();
        }
    }
*/



int readInt()
{
    string ret = getToken();
    
    // check if eof is reached, return -1
    if (!ret.compare("-EOF-"))
        return -1;
    for (int i = 0; ret[i]; i++)
    {
        if (!isdigit(ret[i]))
        {
            __parseerror(0);
            exit(1);
            return 100000;
        }
    }
    int tok = stoi(ret);
    return tok;
}


// reads the next token, checks if it is a proper symbol name and returns corresponding symbol if no error
string readSymbol(){
    string sy= getToken();
    //cout << "entered Read symbol" <<"\n";
   if(!isalpha(sy[0]))
    {   
        __parseerror(1);
        exit(1);
        return "";
    }
    /*if (!sy.compare("EOF"))
    {
        //curr_offset = next_offset;
        __parseerror(1);
        exit(1);
        
    }*/
         
    for(int i =1; i <sy.size();i++){
        if(!isalpha(sy[i]) & !isdigit(sy[i]) || i ==16)
        {
            __parseerror(1);
            exit(0);
            return "" ;
        }
        
    }
    if(sy.size()>16){
        __parseerror(3);
        return "";
    }
    //cout <<sy <<"\n";
    return sy;


}

char  readIAER() {
    string instr =getToken();
    /*if(instr.compare("EOF")){
        curr_offset = next_offset;
        return -1;
   } */
    if(instr.size()!=1 || ( instr[0] != 'I' && instr[0] != 'A' && instr[0] != 'E' && instr[0] != 'R')){
        __parseerror(2);
        exit(1);
         return 0;
    }
    return(instr[0]);



}

void createModule()
{
    struct modules *m = (struct modules *)malloc(sizeof(struct modules));
    if(curr_modules==NULL)
    {
        m->base_add = 0;
        m->length = 0;
        m->mod_num=1;
        curr_modules = m;
    }
    else
    {
        int add = curr_modules->base_add + curr_modules->length;
        m->base_add=add;
        m->length=0;
        m->mod_num = curr_modules->mod_num+1;
        curr_modules=m;
    }
}


void  addNewSymbol(string sym,int val){
  
    //cout << "add symbol entered" << "\n";
    // cout << sym_t.size() << " before the size is  :"<<"\n";
    for (int i = 0; i < sym_t.size(); i++)
    {   //cout << "in for condition " << "\n";
        if (!sym.compare(sym_t[i].name))       // Check for multiple definitions 
        {
            sym_t[i].error = __error__(3);
            
            return;
        }
    }
    SymbolTable sym_details;
    sym_details.name = sym;
    sym_details.used = 0;
    sym_details.relative = val;
    sym_details.error = "";
    sym_details.value=  val + curr_modules->base_add;  //need to add absoute address
    sym_details.module = curr_modules->mod_num;
    sym_t.push_back(sym_details);
    /*cout << sym_t.size() << " the size is  :"<<"\n";
    cout << "the new symbol is:" << sym_details.name<<"\n";
    cout << "the  rel value is:" <<  sym_details.relative<<"\n";*/
    //cout << "the value is:" <<  sym_details.value<<"\n"; 
    
}
 void Parse(char type, int instr)
 {
    
    int used[16]={0};
    int opcode = instr / 1000;
    int operand = instr % 1000;
    //cout << "the opcode is " << opcode << "\n";
    //cout << " the operand is " << operand << "\n";
     
    if(type=='I')

    {
                if(instr>10000)
                {   cout << setfill('0') << setw(3) <<memory_srno << ":";
                    cout << 9999;
                    cout <<  "\t" <<"the error is "<<__error__(4)<<"\n";
                    memory_srno=memory_srno+1;
                    
                }
                else if(opcode>=10)
                {  cout << setfill('0') << setw(3) <<memory_srno << ":";
                    cout << 9999;
                    cout << "\t"<<"the error is "<< __error__(5) <<"\n";
                    memory_srno=memory_srno+1;
                    
                }
                else{
                   cout << setfill('0') << setw(3) <<memory_srno;
                   
                    cout <<": "<<setfill('0') << setw(4)<<instr<<"\n";
                    memory_srno=memory_srno+1;
                }
    }
    else if (type == 'A')
    {
        if (operand > 512)
        {   cout << setfill('0') << setw(3) <<memory_srno;
            cout <<": "<<setfill('0') << setw(4)<<opcode*1000;
            cout <<"" <<"the error is "<<  __error__(0) << "\n";
            memory_srno=memory_srno+1;
            
        }   
        else
        {   cout << setfill('0') << setw(3) <<memory_srno;
            cout <<": "<<setfill('0') << setw(4)<<instr<< "\n";
            memory_srno=memory_srno+1;
            
        }
    }
    else if (type == 'R')
    {    
        //int relativeAddress = modules.baseAddress[currentModule - 1] + operand;
        if (operand >curr_modules->length)
        {
            cout << setfill('0') << setw(3) <<memory_srno;
            cout <<": "<<setfill('0') << setw(4)<<opcode*1000+curr_modules->base_add;
            cout <<"the error is "<<  __error__(1)<< "\n";
            memory_srno=memory_srno+1;
            
        }
        else if (opcode>=10)
        {    
            cout << setfill('0') << setw(3) <<memory_srno<<":";
            cout << 9999;
            cout <<"the error is "<< __error__(5) <<"\n";
             memory_srno=memory_srno+1;
            
        }
        else{
            cout << setfill('0') << setw(3) <<memory_srno;
            cout <<": "<<setfill('0') << setw(4)<<opcode*1000+curr_modules->base_add+operand << "\n";
            memory_srno=memory_srno+1;

        }
    }
    else
    {
        if (operand >= use_list.size())

        {
            cout << setfill('0') << setw(3) <<memory_srno;
            cout <<": "<<setfill('0') << setw(4)<< instr ;
            cout <<"The error is "<<  __error__(2) <<"\n";
            memory_srno=memory_srno+1;
            
            
        }
        else
        {
            //used[operand]=1;
            use_list[operand].used=1;
            //usedSymbolsCheck[operand] = 1;
            int flag = 0;
            for (int i = 0; i < sym_t.size(); i++)
            {
                if (!use_list[operand].name.compare(sym_t[i].name))
                {
                    flag = 1;
                    sym_t[i].used = 1;
                    int address = sym_t[i].value + (opcode*1000);
                    cout << setfill('0') << setw(3) <<memory_srno;
					cout <<": "<<setfill('0') << setw(4)<<address<<"\n" ;
                    memory_srno=memory_srno+1;
                    break;
                    
                }
            }
            if (!flag)
            {   int address = (opcode*1000);
                cout << setfill('0') << setw(3) <<memory_srno;
				cout <<": "<<setfill('0') << setw(4)<<address;
                cout <<"the given error"<<use_list[operand].name <<" is not defined; zero used"<<"\n";
                memory_srno=memory_srno+1;
                
            }
        }
    }
    
    //memory_srno=memory_srno+1;
   //cout << "the value mem " << memory_srno << "\n";


}

void  PrintTable(){
    cout << "SYMBOL TABLE" <<"\n";
    //cout << sym_t.size() << "\n";
    for(int i =0;i<sym_t.size();i++){
       
        cout <<  sym_t[i].name << "=" << sym_t[i].value << ":" << sym_t[i].error << "\n";
    }
}

void  Pass1()
{   vector<string> defSymbols;
    vector<int>defvalues;
    int num_instr=0;
    
    if (!my_file)
    {
        cout << "Error opening file!" << endl;
        exit(0);
    }
    
    while (!my_file.eof()){
        createModule();
        int defCount = readInt();
        //cout << defCount << "\n";
        if (defCount == -1)
                break;

        
        if (defCount > 16)

        {
                __parseerror(4);
                exit(1);
        }
            
        for(int i=0;i<defCount;i++)
        {
            string s = readSymbol();
            //cout << "sym done" << "\n";
            defSymbols.push_back(s);
            //cout << defSymbols << " symbols are " << "\n";
            //cout  << defSymbols.size() << "no of symbols in defcount are re"<<"\n";
            int val = readInt();
            defvalues.push_back(val);
                
            
            //cout << "int value printed" << "\n";
            addNewSymbol(s, val);
    
                
                
        }
             
        
        int usecount = readInt();

        
           /* if(usecount==-1)
            {
                
                __parseerror(0);
            }*/
    //        printf("%d \n",usecount);
            if(usecount>16)
            {
                __parseerror(5);
                exit(1);
            }
            use_list.clear();
            for(int i=0;i<usecount;i++)
            {
                string sym = readSymbol();
                SymbolTable use_details;
                use_details.name=sym;
                
                use_list.push_back(use_details);
    //            printf("%s \n",sym->name);
            }
            int instrCount = readInt();
            num_instr+=instrCount;
            if (num_instr > 512)
            {
            __parseerror(6);
            exit(1);
            }
            //curr_modules->base_add=curr_modules->base_add+instrCount;
            for (int i = 0; i < instrCount; i++)
            {
                char aMode = readIAER();
                int instruction = readInt();
                curr_modules->length+=1;
            
            }
            
            struct SymbolTable *temp;
            temp = symbolTable;
            //cout << "the current module length is" << curr_modules->length <<"\n";
            // Print out warning 1 before symbol table
           

            vector<SymbolTable>c_mod(sym_t.begin(),sym_t.end());
           for (int i = 0; i < defSymbols.size(); i++)
            {
                for (int j = 0; j < sym_t.size(); j++)
                {    //cout <<sym_t[j].name<<"\n";
                     

                    
                    if (!defSymbols[i].compare(sym_t[j].name))
                    {
                       for(int k=0;c_mod[j].module == curr_modules->mod_num;k++)
                        
                    { 
                        
                           // <<sym_t[j].relative <<"\n";
                           // cout <<curr_modules->length<<"\n";
                        if (sym_t[j].relative>curr_modules->length) 
                        { 
                            
                            cout << "Warning: Module " << sym_t[j].module<< ": " << sym_t[j].name 
                            <<" too big " << sym_t[j].relative << "(max= "  
                            << curr_modules->length-1  << ")"<< " assume zero relative" << "\n";
                            sym_t[j].value = curr_modules->base_add;
                            sym_t[j].relative = 0;
                        }
                        c_mod[k].module++;
                    }
                   
                    }
                }
                
            
            }
            
            
            
            

    }
}


void Pass2()
{   
    if (!my_file)
    {
        cout << "Error opening file!" << endl;
        exit(0);
    }
    int num_instr =0;
    cout<<"\n"<<"Memory Map"<<"\n";
    while(!my_file.eof()){

        createModule();
        
        int defCount = readInt();
        
         if (defCount == -1)
                break;
    
     
         if (defCount > 16)
        {
            __parseerror(4);
            exit(1);
        }
        for(int i=0;i<defCount;i++)
        {
            string  sym = readSymbol();
            
            int val = readInt();
            /*if(val==-1)
            {
                
                __parseerror(0);
            }
            */
            
        }
        




    
        int usecount = readInt();
        if(usecount>16)
            {
                __parseerror(5);
            }
        //cout << "the uselis size before" << use_list.size() <<"\n";
        use_list.clear();
        for (int i =0;i<usecount;i++){

            string sym=readSymbol();
            SymbolTable use_details;
            use_details.name=sym;
            use_details.used=0;
            use_list.push_back(use_details);
            //cout << use_details.name<< "\n";
            //cout << use_details.used << "\n";
        }
        int instrCount = readInt();
    
        num_instr+=instrCount;
        if(num_instr>512)
        {
            __parseerror(6);
        }
        curr_modules->length+=instrCount;
        for (int i = 0; i < instrCount; i++)
        {
            char aMode = readIAER();
            

            int instruction = readInt();
            //cout <<  instruction << "\n";
           // cout << " entering parse mode " << "\n";

            Parse(aMode, instruction);
        }
        //cout << "the size is"<<use_list.size() <<"\n";
        for(int i = 0; i < use_list.size(); i++) 
        {       
				if(use_list[i].used == 0) {
					cout<<"Warning: Module "<<curr_modules->mod_num<<": "<<use_list[i].name<<" appeared in the uselist but was not actually used\n";	
				}
	    }
       
        
        }
        

    

}


int main(int argc, char** argv)
{

file = argv[1];

my_file.open(file);

Pass1();

PrintTable();

my_file.close();
curr_modules=NULL;
my_file.open(file);
Pass2();
 cout<<"\n";
        
        for(int i = 0; i < sym_t.size(); i++) 
        {

            
			    if(sym_t[i].used == 0)
                 {
                    
                     
                        cout<<"Warning: Module "<< sym_t[i].module<<": "<<sym_t[i].name<<" was defined but never used\n";
                     
                     
				
			    }
             
            
    
        }
my_file.close();


}

