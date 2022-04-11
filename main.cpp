#include <stdio.h>
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <string.h>
#include <sstream>
#include <string>
#include <iomanip>
#include<vector>
#include <deque>


using namespace std;

void __parseerror(int errcode, int linenum, int lineoffset) {
        char errstr[7][50] = {
            "NUM_EXPECTED", // Number expect
            "SYM_EXPECTED", // Symbol Expected
            "ADDR_EXPECTED", // Addressing Expected which is A/E/I/R
            "SYM_TOO_LONG", // Symbol Name is too long
            "TOO_MANY_DEF_IN_MODULE", // > 16
            "TOO_MANY_USE_IN_MODULE", // > 16
            "TOO_MANY_INSTR"  // total num_instr exceeds memory size (512)
        };
        cout <<"Parse Error line "<< linenum<<" offset "<< lineoffset <<": "<<errstr[errcode];
}

class Symbol {
    public:
    string txt="";
    int val;
    string msg="";
    int module_no;
    bool used;
};

class token {
    public:
    int linenum;
    int offset;
    int num = -1;
    char* txt;
    token(int l, int o, char* t){
        linenum = l;
        offset = o;
        txt = t;
    }
};

ifstream f;         

class Mytokenizer {
    private:
        string line = "";
        char *tok = NULL; 
        char *temp = NULL;
        int lineno=0, pos=0, line_length;

        token getToken() {
            if (tok == NULL) {
                if (  !f.eof() ) {
                    
                    getline(f, line);
                    tok = strtok(&line[0], " \t\n");
                    if (tok == NULL) {
                        
                        lineno++;
                        return getToken();
                    }
                    lineno++;
                    pos=tok - &line[0] + 1;
                    line_length = line.length();
                    //printf("Token: %d:%d :  %s\n",lineno, pos,tok);
                    return token(lineno, pos, tok);

                } else if (f.eof()) {
    
                    //printf("Final Spot in File: line=%d offset=%d", lineno, line_length);
                    f.close();
                    return token(--lineno, line_length+1, NULL);
                }
                
            } else {
                tok = strtok(NULL, " \t\n");
                if (tok == NULL) {
                    return getToken();
                }else {
                    pos = tok - &line[0] + 1;
                    //printf("Token: %d:%d :  %s\n",lineno, pos,tok);                    
                    return token(lineno, pos, tok);
                }
                
            }       
    }

    public:
    Mytokenizer(char *fileloc) {
        f.open(fileloc);
    }


    token readInt() {
        token tk = getToken();
       
        if (tk.txt != NULL) {
            for (int i =0; i< strlen(tk.txt);i++){
                if (!isdigit(tk.txt[i])) {
                    __parseerror(0, tk.linenum, tk.offset);
                    exit(0);
                }
            }
            tk.num = stoi(tk.txt);
            return tk;
        }
    }

    token readSymbol() {
        token tk = getToken();
        if (tk.txt != NULL) {
            // symbol length atmost 16
            if ( strlen(tk.txt) <=16) {
                if (isalpha(tk.txt[0])) {

                    for (int i =1; i<strlen(tk.txt); i++){
                        if(!isalnum(tk.txt[i])) {
                            
                            __parseerror(1, tk.linenum, tk.offset);
                            exit(0);
                        }
                    }
                    return tk;
                }
            } else {
                __parseerror(3, tk.linenum, tk.offset);
                exit(0);
            }
        } else {
            __parseerror(1, tk.linenum, tk.offset);
            exit(0);
        }

    }

    token readIAER() {
        token tk = getToken();

        if (tk.txt != NULL) {
            if (strcmp(tk.txt,"I")== 0 || strcmp(tk.txt,"A") == 0 || strcmp(tk.txt,"E") == 0 || strcmp(tk.txt,"R") == 0 ) {
                return tk;
            }

        } else {
            __parseerror(2, tk.linenum, tk.offset);
            exit(0);
        }
    }

    void check() {
        // while ( f.is_open()){
        //    token t = readSymbol() ;
        //     cout << t.txt << endl;
        // }
        // token t = readIAER();
        // token t1 = readSymbol();
        // cout << t.txt << endl;
    }

};

vector<Symbol> symbol_table;
vector<Symbol> uselist;

void create_symbol(token symbol, int val, int mod){
    
    // check if prev defined.
    for (int i =0; i< symbol_table.size(); i++) {
        if (symbol_table[i].txt.compare(symbol.txt) == 0) {
            symbol_table[i].msg = "Error: This variable is multiple times defined; first value used";
            return;
        }
    }
    
    Symbol s;
    s.txt = symbol.txt;
    s.val = val;
    s.module_no = mod;
    symbol_table.push_back(s); 
}

void print_table(){
    cout << "Symbol Table" << endl;
    for (int i=0;i<symbol_table.size();i++){
       cout << symbol_table[i].txt << "=" << symbol_table[i].val;
       if (symbol_table[i].msg != "") { cout << " " << symbol_table[i].msg;} 
       cout << endl;
    }
    cout << endl;
}


void parser(char addr_mode, int op, int address, int intrcount) {

    //cout << " checking" << op << endl;

    int opcode = int(op / 1000);
    int operand = op % 1000;
    int abs_zero = opcode*1000;
    //cout << addr_mode;
    if (addr_mode == 'I') {
        if (10000 <= op) {
            cout << ": " << setfill('0') << setw(4) << 9999;
			cout << " Error: Illegal immediate value; treated as 9999" << endl;
        }
        else{
			cout << ": " << setfill('0') << setw(4) << op <<endl;
		}
    }
    else if (opcode >= 10) {
        cout << ": " << setfill('0') << setw(4) << 9999;
		cout << " Error: Illegal opcode; treated as 9999" << endl;
    }
     
    else if (addr_mode == 'A') {
        int machine_size = 512;
        if (operand <= machine_size) {
			cout << ": " << setfill('0') << setw(4) << op << endl;
		}
		else {
            cout << ": " << setfill('0') << setw(4) << abs_zero;     // using absolute zero 
            cout << " Error: Absolute address exceeds machine size; zero used" << endl;
        }
    }

    else if (addr_mode == 'E') {
        // check operand size
        if (operand >= uselist.size()) {
            cout << ": " << setfill('0') << setw(4) << op;
            cout << " Error: External address exceeds length of uselist; treated as immediate" << endl;
		}
        else {
            uselist[operand].used = true;
            string x = uselist[operand].txt;
            bool found = false;

            for (int i=0;i< symbol_table.size(); i++) {
                if (x.compare(symbol_table[i].txt) == 0) {
                    symbol_table[i].used = true;
                    cout << ": " << setfill('0') << setw(4) << abs_zero + symbol_table[i].val << endl;
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << ": " << setfill('0') << setw(4) << abs_zero;
				cout << " Error: " << x << " is not defined; zero used" << endl;
            } 
            
        }
    }

    else if (addr_mode == 'R') {
        if (intrcount < operand) {
            cout << ": " << setfill('0') << setw(4) << abs_zero + address;
			cout << " Error: Relative address exceeds module size; zero used" << endl;
        }
        else {	
			cout << ": " << setfill('0') << setw(4) << address + op << endl;		
		}
    }

}

void pass_one(Mytokenizer *t){
    int address = 0,module_no = 1;

    while (!f.eof()) {
        token defcount = t->readInt();

        if (defcount.num < 0) {break;}

        if(defcount.num > 16) {
            __parseerror(4, defcount.linenum, defcount.offset);
            exit(0);			
		}

        for (int i=0; i < defcount.num; i++) {
           
            token symbol = t->readSymbol();
            int rel_addr = t->readInt().num;

            create_symbol(symbol, address+rel_addr, module_no); 
        }

        token usecount = t->readInt();

        if(usecount.num > 16) {
            __parseerror(5, usecount.linenum, usecount.offset);
            exit(0);			
		}

        for (int i=0; i< usecount.num;i++) {
                t->readSymbol();
            }

        token intrcount = t->readInt();
                
        // check addresses
        for (int i=0; i < symbol_table.size();i++) {
            if (symbol_table[i].module_no == module_no){
                if ((symbol_table[i].val - address) > intrcount.num -1) {
                    cout << "Warning: Module " << module_no << ": " << symbol_table[i].txt << " too big " << (symbol_table[i].val - address) << " (max=" << intrcount.num-1<< ") assume zero relative" << endl;
                    symbol_table[i].val = address;
                }
            }
            
        }


        address += intrcount.num;

        if(address > 512) {
				__parseerror(6, intrcount.linenum, intrcount.offset);
				exit(0);
		}

        for ( int i=0; i< intrcount.num;i++) {
           t->readIAER();
            t->readInt();
        }

        module_no++;
    }
    print_table();
}

void pass_two(Mytokenizer *t) {
    int address=0, module_no = 1;

    cout<<"Memory Map"<<endl;

    while ( !f.eof() ) {
        token defcount = t->readInt();

        if (defcount.num < 0) {break;}

        for (int i=0; i < defcount.num; i++) {
            token symbol = t->readSymbol();
            int rel_addr = t->readInt().num;

            //create_symbol(symbol, address+rel_addr, module_no); 
        }

        token usecount = t->readInt();

        for (int i=0; i< usecount.num;i++) {
            token s = t->readSymbol();
            Symbol for_use;
            for_use.module_no = module_no;
            for_use.txt = s.txt;
            for_use.used = false;
            uselist.push_back(for_use);
        
        }

        token intrcount = t->readInt();
        
        for(int i=0;i<intrcount.num;i++) {
            cout<<setfill('0') << setw(3) << i + address;  
            char addressmode = t->readIAER().txt[0];
            int operand = t->readInt().num;
            parser(addressmode, operand, address, intrcount.num);
        }

        // msg for declared but not used 
        for (int i=0; i< uselist.size(); i++) {
            if (!uselist[i].used) {
                cout << "Warning: Module " << module_no << ": " << uselist[i].txt << " appeared in the uselist but was not actually used" << endl;	
            }
        }

        address += intrcount.num;
        module_no++;
        uselist.clear();
    }
    cout << endl;
    // msg for defined but not used.
    for (int i=0 ; i< symbol_table.size();i++) {
        //cout << symbol_table[i].txt << " : " << symbol_table[i].used << endl;
        if(symbol_table[i].used == false) {
            cout << "Warning: Module " << symbol_table[i].module_no << ": " << symbol_table[i].txt << " was defined but never used" << endl;
        } 
    }
}


main(int argc, char* argv[]) {

    if (argc != 2) {
         printf("\nGive me My precious! arguments\n");
         exit(1);
     }

     char *in_file = argv[optind++];
     Mytokenizer *t = new Mytokenizer(in_file);
     
     //t->check();
     pass_one(t);
     f.close();
     Mytokenizer *t2 = new Mytokenizer(in_file);
     pass_two(t2);

}