void A(int S, int E, char *p)  // PARSE FEN
{
    for(int i = 0; i < 129; i++) b[i] = 0;

    int f = 0; int r = 7;
    
    do
    {
        if((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z')) b[(7 - r) * 16 + f++] = c[*p];
        if(*p >= '0' && *p <= '9') f += *p - '0';
        if(*p == '/') { f = 0; r--; }
    }
    
    while(*p++ != ' ');
    
    (*p++ == 'w') ? (S = 8) : (S = 16); *p++;
    
    do
	{
		//TODO set virgin bits on corner pieces
	}
	
	while(*p++ != ' ');

	if(*p != '-') E = (7 - ((p[1] - '0') - 1)) * 16 + (p[0] - 'a');
}
