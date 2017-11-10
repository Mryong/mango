string func(){
	string str = "abcd\n";
	return str;
}

delegate string func_var();
func_var = func;

string abc = func_var();
print(abc);
