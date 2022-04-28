In general, a linker takes individually compiled code/object modules and creates a single executable by resolving external symbol references 
(e.g. variables and functions) and module relative addressing by assigning global addresses after placing the modules’ object code at global addresses.
The input to the linker is a file containing a sequence of tokens (symbols and integers and instruction type characters). Don’t assume tokens that make 
up a section to be on one line, don’t make assumptions about how much space separates tokens or that lines are non-empty for that matter or that each 
input conforms syntactically.
