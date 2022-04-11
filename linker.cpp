#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector> 
#include <cstdlib>
#include <iomanip>

using namespace std;

void __parseerror(int errcode, int linenum, int lineoffset);

typedef struct token {
	char* token_text;
	int linenum;
	int lineoffset;
} token;

typedef struct num_token {
	int val;
	int linenum;
	int lineoffset;
} num_token;

typedef struct sym_struct {
	char token_text[20];
	char* error_msg;
	int used, mod_no;
	int linenum;
	int lineoffset;
	int val;
} sym_struct;

//wrapping getToken up
class Tokenizer {
	private:
	int curr_linenum;
	int curr_lineoffset;
	int prev_line_len;
	ifstream input_file;
	char* curr_token;
	string curr_line;

	public:
	
	Tokenizer(char* file_name) {
		curr_linenum = 0;
		curr_token = NULL;
		curr_line = "";
		curr_lineoffset = 0;
		input_file.open(file_name);
		if (!input_file.is_open()) {
			cout << "Error: Cannot open file";
		}
	}

	token getToken() {
	  	if (input_file.is_open())
	  	{
		    	if(curr_token == NULL)
			{
				if(!input_file.eof())
				{
					while(curr_token == NULL && !input_file.eof()) {
						prev_line_len = curr_line.size();
						getline(input_file,curr_line);
						curr_token = strtok(&curr_line[0], " \t\n");
						curr_linenum++;
						curr_lineoffset = 0;
					}

					if(input_file.eof()) {
						input_file.close();
						curr_linenum -= 1;
						token temp = {NULL, curr_linenum, prev_line_len + 1};
						return temp;					
					}
				}
				else
				{
					input_file.close();
					curr_linenum -= 1;
					token temp = {NULL, curr_linenum, prev_line_len};
					return temp;
				}
			}
			else
			{
				curr_token = strtok(NULL, " \t\n");
				if(curr_token == NULL)
					return getToken();
			}
			curr_lineoffset = curr_token - &curr_line[0] + 1;
			token temp = {curr_token, curr_linenum, curr_lineoffset};
			return temp;
	  	}
		else
		{
			token temp = {NULL, -1, -1};
			return temp;
		}
	}
	
	
	token readIAER() {
		token tok = getToken();
		if(tok.token_text != NULL) {
			if(strlen(tok.token_text) == 1 &&
				(strcmp(tok.token_text, "I") == 0 || strcmp(tok.token_text, "A") == 0
				|| strcmp(tok.token_text, "E") == 0 || strcmp(tok.token_text, "R") == 0)) 
			{
				token temp;
				temp.token_text = (char*)malloc(17*sizeof(char));
				strcpy(temp.token_text,tok.token_text);
				temp.linenum = tok.linenum;
				temp.lineoffset = tok.lineoffset;			
				return temp;
			}
		}	
		__parseerror(2, tok.linenum, tok.lineoffset);
		exit(0);			
	}
	
	token readSymbol() {
		token tok = getToken();
		if(tok.token_text != NULL) {
			if(strlen(tok.token_text) > 0 && strlen(tok.token_text) < 17) {
				if(isalpha(tok.token_text[0])) {
					for (int i = 1; i < strlen(tok.token_text); i++)
					{
						if (isalnum(tok.token_text[i]) == false)
						{
							__parseerror(1, tok.linenum, tok.lineoffset);
							exit(0);
						}
					}			
					token temp;
					temp.token_text = (char*)malloc(17*sizeof(char));
					strcpy(temp.token_text,tok.token_text);
					temp.linenum = tok.linenum;
					temp.lineoffset = tok.lineoffset;			
					return temp;
				}
			}
			else {
				__parseerror(3, tok.linenum, tok.lineoffset);
				exit(0);
			}
		}
		__parseerror(1, tok.linenum, tok.lineoffset);
		exit(0);	
	}

	num_token readInt() {
		token tok = getToken();
		if(tok.token_text != NULL) {
			for (int i = 0; i < strlen(tok.token_text); i++)
			{
				if (isdigit(tok.token_text[i]) == false)
				{
					__parseerror(0, tok.linenum, tok.lineoffset);
					exit(0);			
				}
			}
			int x;
			sscanf(tok.token_text, "%d", &x);
			num_token temp = {x, tok.linenum, tok.lineoffset};
			return temp;
		}
		else
		{
			num_token temp = {-1, tok.linenum, tok.lineoffset};
			return temp;					
		}
	}

	void closeFile() {
		if(input_file.is_open()) {
			input_file.close();
		}
	}
	
};

class Parser {

	private:
	char* file;
	Tokenizer *t;
	vector<sym_struct> sym_table;
	vector<sym_struct> use_list;
	
	public:
	Parser(char* file_name) {
		file = file_name;
		t = new Tokenizer(file);
	}

	void createSymbol(token symbol, int val) {		
		for(int i = 0; i < sym_table.size(); i++) {
			if(strcmp(sym_table[i].token_text, symbol.token_text) == 0) {
				sym_table[i].error_msg = (char*)malloc(70*sizeof(char));
				strcpy(sym_table[i].error_msg,"Error: This variable is multiple times defined; first value used");			
				return;
			}
		}

		sym_struct sym;
		sym.used = 0;
		sym.mod_no = 0;		
		strcpy(sym.token_text, symbol.token_text);
		sym.linenum = symbol.linenum;
		sym.lineoffset = symbol.lineoffset;
		sym.val = val;
		sym.error_msg = NULL;
		
		sym_table.push_back(sym);
		
		if(sym_table.size() > 16) {
			__parseerror(4, sym.linenum, sym.lineoffset);
			exit(0);
		}		
	}

	void parseContent(token instr, int op, int cur_address, int instcount) {
		
		int opcode = (int) op/1000;
		int operand = op % 1000;	
	
		if(strcmp(instr.token_text, "I") == 0) {
			if(op >= 10000) {
				cout <<": "<<setfill('0') << setw(4)<<9999;
				cout<< " Error: Illegal immediate value; treated as 9999" << "\n";
			}
			else	{
				cout <<": "<<setfill('0') << setw(4)<<op<<"\n";
			}	
		}
		else if(opcode >= 10) {
			cout <<": "<<setfill('0') << setw(4)<<9999;
			cout<< " Error: Illegal opcode; treated as 9999" << "\n";
		}
		else if(strcmp(instr.token_text, "A") == 0) {
			if(operand <= 512) {
				cout <<": "<<setfill('0') << setw(4)<<op << "\n";
			}
			else {
				cout <<": "<<setfill('0') << setw(4)<<opcode*1000;
				cout << " Error: Absolute address exceeds machine size; zero used" << "\n";
			}
		}
		else if(strcmp(instr.token_text, "E") == 0) {
			if(operand >= use_list.size()) {
				cout <<": "<<setfill('0') << setw(4)<<op;
				cout << " Error: External address exceeds length of uselist; treated as immediate" << "\n";
			}
			else {
				char* sym = use_list[operand].token_text;
				use_list[operand].used = 1;
				int val;
				int flag = 0;
				for(int i = 0; i < sym_table.size(); i ++) {
					if(strcmp(sym_table[i].token_text, sym) == 0) {
						val = sym_table[i].val;
						sym_table[i].used = 1;
						flag = 1;
						break;
					}				
				}
				if(flag == 1) {
					int address = val + (opcode*1000);
					cout <<": "<<setfill('0') << setw(4)<<address << "\n";
				}	
				else {
					int address = (opcode*1000);
					cout <<": "<<setfill('0') << setw(4)<<address;
					cout << " Error: " << sym << " is not defined; zero used"<< "\n";				
				}		
			}
		}
		else if(strcmp(instr.token_text, "R") == 0) {
			if(operand > instcount) { 
				cout <<": "<<setfill('0') << setw(4)<<opcode*1000+cur_address;
				cout << " Error: Relative address exceeds module size; zero used" << "\n";
			}
			else {	
				cout <<": "<<setfill('0') << setw(4)<<opcode*1000+cur_address+operand << "\n";		
			}
		}
	}

	void pass_1() {
		int cur_address = 0;
		num_token defcount = t->readInt();
		int mod_no = 1;	
		int prev_defcount_total = 0;
		while (defcount.val != -1) {
			
			if(defcount.val > 16) {
				__parseerror(4, defcount.linenum, defcount.lineoffset);
				exit(0);			
			}
			for (int i=0;i<defcount.val;i++) {
				token symbol = t->readSymbol();
				num_token relative = t->readInt();
	
				int val = cur_address + relative.val;
				createSymbol(symbol,val);
			}
	
			num_token usecount = t->readInt();

			if(usecount.val > 16) {
				__parseerror(5, usecount.linenum, usecount.lineoffset);
				exit(0);			
			}

			use_list.clear();
			for (int i=0;i<usecount.val;i++) {
				token symbol = t->readSymbol();

				sym_struct temp;
				strcpy(temp.token_text, symbol.token_text);
				temp.linenum = symbol.linenum;
				temp.lineoffset = symbol.lineoffset;
				use_list.push_back(temp);				
				if(use_list.size() > 16) {
					__parseerror(5, temp.linenum, temp.lineoffset);
					exit(0);			
				}			
			}
		
			num_token instcount = t->readInt();	
			for(int i = prev_defcount_total; i < prev_defcount_total + defcount.val; i++) {
				int relative = sym_table[i].val - cur_address;
								
				if(relative > instcount.val - 1) {
					cout<<"Warning: Module "<<mod_no<<": "<<sym_table[i].token_text<<" too big "<<relative<<" (max="<<instcount.val-1<<") assume zero relative\n";
					sym_table[i].val = cur_address;		
				}
			}

			cur_address += instcount.val;

			if(cur_address > 512) {
				__parseerror(6, instcount.linenum, instcount.lineoffset);
				exit(0);
			}
			
			token instr;
			num_token operand;

			for (int i=0;i<instcount.val;i++) {
				instr = t->readIAER();
				operand = t->readInt();	
			}
			
			mod_no++;
			prev_defcount_total += defcount.val;
			defcount = t->readInt();
		}
		printSymbolTable();
	}

	void pass_2() {
		if(t != NULL) t->closeFile();
		
		t = new Tokenizer(file);

		cout<<"\n"<<"Memory Map"<<"\n";

		int cur_address = 0;
		int mod_no = 1;
		int defcount = t->readInt().val;
		int prev_defcount_total = 0;
		while (defcount != -1) {
			
			for (int i=0;i<defcount;i++) {
				token symbol = t->readSymbol();
				int val = cur_address + t->readInt().val;
				sym_table[prev_defcount_total + i].mod_no = mod_no;			
			}
	
			int usecount = t->readInt().val;
			use_list.clear();
			for (int i=0;i<usecount;i++) {
				token symbol = t->readSymbol();
				sym_struct temp;
				strcpy(temp.token_text, symbol.token_text);
				temp.linenum = symbol.linenum;
				temp.lineoffset = symbol.lineoffset;
				temp.used = 0;
				use_list.push_back(temp);
			}
		
			int instcount = t->readInt().val;
			for (int i=0;i<instcount;i++) {
				cout<<setfill('0') << setw(3) << cur_address + i;  
				
				token instr = t->readIAER();
				int operand = t->readInt().val;
				parseContent(instr, operand, cur_address, instcount);
			}

			for(int i = 0; i < use_list.size(); i++) {
				if(use_list[i].used == 0) {
					cout<<"Warning: Module "<<mod_no<<": "<<use_list[i].token_text<<" appeared in the uselist but was not actually used\n";	
				}
			}

			mod_no++;
			prev_defcount_total += defcount;
			defcount = t->readInt().val;			
			cur_address += instcount;
		}

		cout<<"\n";
		for(int i = 0; i < sym_table.size(); i++) {
			if(sym_table[i].used == 0) {
				cout<<"Warning: Module "<< sym_table[i].mod_no<<": "<<sym_table[i].token_text<<" was defined but never used\n";
			}
		}

	}

	void printSymbolTable() {
		cout<<"Symbol Table"<<"\n";
		for(int i = 0; i < sym_table.size(); i++) {
			cout<<sym_table[i].token_text<<"="<<sym_table[i].val;
			if(sym_table[i].error_msg != NULL)
				cout<<" "<<sym_table[i].error_msg;
			cout<<"\n";		
		}
	}
};

int main(int argc, char** argv)
{
	if(argc != 2) {
		cout << "Invalid arguments.Try `./linker.exe <filename>`";
		return 0;
	}
	Parser *parser = new Parser(argv[1]);
	parser->pass_1();
	parser->pass_2();

	return 0;
}

void __parseerror(int errcode, int linenum, int lineoffset) {
	static char errstr[7][30] = {
		"NUM_EXPECTED", // Number expect
		"SYM_EXPECTED", // Symbol Expected
		"ADDR_EXPECTED", // Addressing Expected which is A/E/I/R
		"SYM_TOO_LONG", // Symbol Name is too long
		"TOO_MANY_DEF_IN_MODULE", // > 16
		"TOO_MANY_USE_IN_MODULE", // > 16
		"TOO_MANY_INSTR" // total num_instr exceeds memory size (512)
	};
	printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode]);
}
