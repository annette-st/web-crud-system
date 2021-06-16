def print_templ_hdr(n):
    s = "template <";
    start = True;
    for i in range (0, n):
        if not start:
            s = s + ", ";
        else:
            start = False;
        s = s + "typename T" + str(i);
    s = s + ">";
    return s

def print_func_args(n):
    s = "void print(";
    start = True;
    for i in range (0, n):
        if not start:
            s = s + ", ";
        else:
            start = False;
            
        s = s + "const T" + str(i) + "& a" + str(i);
    s = s +")";
    return s;

def print_func_body(n):
    s = "{\n";
    s = s + " if(stream) {";
    s = s + " prolog();\n(*stream) ";
    start = True;
    for i in range (0, n):
        s = s + "<< ";
        if not start:
            s = s + "\" \" << ";
        else:
            start = False;
        s = s + "a" + str(i);
    s = s + ";\nepilog();}}";
    return s;

for i in range(1, 19):
    print print_templ_hdr(i);
    print print_func_args(i);
    print print_func_body(i);
    
