#include <iostream>
#include <cstring>
#include <queue>
#include <iomanip>
#include <vector>
#define NUM_OF_KWORDS 8
#define NUM_OF_TYPES 17

using namespace std;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

string keywords[NUM_OF_KWORDS] = {"create","drop","alter","table","column","add",","};
string typewords[NUM_OF_TYPES] = { "bit","date","time","timestamp","decimal","real","float","smallint","integer","interval","character"};

enum states {ST,ID,DLM,ERR};
enum tok_names { KWORD, IDENT, TYPE, create, drop, alter, table, column, add, UNKNOW, O, C, END, ENDLINE, comma};

typedef struct token_lexeme
{
	enum tok_names token;
	char lexemes_tok;
}token_lexeme;

vector <string> lexem;

vector <string> token;

int pumpover(string* buf_tok, string* buf_lex)
{
	string temp[256];
	int size = 0;
	while (!token.empty())
	{
		buf_tok[size] = token.front();
		token.pop_back();
		size++;
	}
	size = 0;
	while (!lexem.empty())
	{
		buf_lex[size] = lexem.front();
		lexem.pop_back();
		size++;
	}
	for (int i = size; i >=0; i--)
	{
		temp[i] = buf_tok[i];
	}
	for (int i = 0; i < size; i++)
	{
		token.push_back(temp[i]);
	}
	for (int i = size; i >= 0; i--)
	{
		temp[i] = buf_lex[i];
	}
	for (int i = 0; i < size; i++)
	{
		lexem.push_back(temp[i]);
	}
	return size;
}
int is_key(const char* id)
{
	string buf;
	buf = id;
	for (int i = 0; i < NUM_OF_KWORDS; i++) if (buf==keywords[i]) return 1;
    for(int j = 0; j < NUM_OF_TYPES; j++) if (buf == typewords[j]) return 2;
	return 0;
}
int lexer(const char* filename)
{
	FILE* fd;
	char c, err_symbol;

	if ((fd=fopen(filename, "r")) == NULL)
	{
		cout << "Cannot open file " << filename << endl;
		return -1;
	}
	enum states CS = ST;

	int size = 0;
	char buf[256];
	buf[size] = 0;
	c = fgetc(fd);
	while (!feof(fd))
	{
		switch (CS)
		{
		case ST:
		{
			while ((c == '\t') || (c == '\n'))
			{
				c = fgetc(fd);
			}
			
			if (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')) || (c == '_'))
			{
				size = 0;
				buf[size] = 0;
				buf[size] = c;
				size++;
				CS = ID;
			}
			else if ((c == ' ') || (c == ';') || (c == ',') || (c == '(') || (c == ')'))
			{
				if (lexem.empty())
				{
					c = fgetc(fd);
					break;
				}
				if (lexem.back() == " " && c != '(' && c != ')')
				{
					c = fgetc(fd);
					break;
				}
				CS = DLM;
			}
			else  continue;

		}
		case DLM:
		{
			if (c==' '||c == ';'||c=='('||c==')'||c==',')
			{
				size = 0;
				buf[size] = 0;
				buf[size] = c;
				size++;
				buf[size] = '\0';
				lexem.push_back(buf);
				token.push_back("DLM");
					c = fgetc(fd);
					CS = ST;
					break;
			}
		}
		case ID:
		{
			c = fgetc(fd);
			while (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')) || (c == '_'))
			{
				buf[size] = c;
				size++;
				c = fgetc(fd);
			}
			buf[size] = '\0';
			if (is_key(buf)==1)       token.push_back("KWORD");
			else if(is_key(buf) == 2) token.push_back("TYPE");
			else					  token.push_back("IDENT");
			lexem.push_back(buf);
			CS = ST;
			break;
		}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tok_names get_next_token(int &count_next_tok)
{
	tok_names tok;
	if (token[count_next_tok] == "create")
	{
		count_next_tok++;
		return tok = create;
	}

	if (token[count_next_tok] == "IDENT")
	{
		count_next_tok++;
		return tok = IDENT;
	}
	if (token[count_next_tok] == "add")
	{
		count_next_tok++;
		return tok = add;
	}

	if (token[count_next_tok] == "TYPE")
	{
		count_next_tok++;
		return tok = TYPE;
	}

	if (token[count_next_tok] == "drop")
	{
		count_next_tok++;
		return tok = drop;
	}
	if (token[count_next_tok] == "alter")
	{
		count_next_tok++;
		return tok = alter;
	}
	if (token[count_next_tok] == "table")
	{
		count_next_tok++;
		return tok = table;
	}

	if (token[count_next_tok] == "column")
	{
		count_next_tok++;
		return tok = column;
	}

	if (token[count_next_tok] == "(")
	{
		count_next_tok++;
		return tok = O;
	}

	if (token[count_next_tok] == ")")
	{
		count_next_tok++;
		return tok = C;
	}

	if (token[count_next_tok] == ";")
	{
		count_next_tok++;
		return tok = END;
	}
	if (token[count_next_tok] == ",")
	{
		count_next_tok++;
		return tok = comma;
	}
	if (token[count_next_tok] == "ERR")
	{
		count_next_tok++;
		return tok = UNKNOW;
	}

	if (token[count_next_tok] == "-|")
	{
		count_next_tok++;
		return tok = ENDLINE;
	}
	return tok = UNKNOW;

}

int FIELD_DEF(tok_names &tok, int &count_next_tok_local)
{
	tok_names FIELD_DEF_tok = tok;
	switch (FIELD_DEF_tok)
	{
	case ID:
		cout << "Read 'ID' successful" << endl;
		FIELD_DEF_tok = get_next_token(count_next_tok_local);
		switch (FIELD_DEF_tok)
		{
		case TYPE:
			cout << "Read 'type' successful" << endl;
			return 1;
		default:
			cout << "Error, incorrect 'type' entry!" << endl;
			return 0;
		}
	default:
		cout << "Error, incorrect 'id' entry!" << endl;
		return 0;
	}
}
int FIELD_DEF_LIST(tok_names &tok, int &count_next_tok_local)
{
	tok_names FIELD_DEF_LIST_tok = tok;
	switch (FIELD_DEF_LIST_tok)
	{
	case ID:
		if (FIELD_DEF(tok, count_next_tok_local))
		{
			FIELD_DEF_LIST_tok = get_next_token(count_next_tok_local);
			if (FIELD_DEF_LIST_tok == comma)
			{
				cout << "Read ',' successful" << endl;
				FIELD_DEF_LIST_tok = get_next_token(count_next_tok_local);
				FIELD_DEF_LIST(FIELD_DEF_LIST_tok, count_next_tok_local);
				return 1;
			}
			else
			{
				count_next_tok_local -= 1;
				return 1;
			}
		}
		else return 0;
	default:
		return 0;
	}
}
int CREATE_STM(tok_names tok, int& count_next_tok_local)
{
	if (count_next_tok_local <= 1)count_next_tok_local += 1;
	tok_names create_tok = tok;
	switch (create_tok)
	{

	case create:
		cout << "Read 'create' successful" << endl;
		create_tok = get_next_token(count_next_tok_local);
		switch (create_tok)
		{
		case table:
			cout << "Read 'table' successful" << endl;
			create_tok = get_next_token(count_next_tok_local);
			switch (create_tok)
			{
			case ID:
				cout << "Read 'ID' successful" << endl;
				create_tok = get_next_token(count_next_tok_local);
				switch (create_tok)
				{
				case O:
					cout << "Read '(' successful" << endl;
					create_tok = get_next_token(count_next_tok_local);
					switch (create_tok)
					{
					case ID:
						if (FIELD_DEF_LIST(create_tok, count_next_tok_local)) 
						{
							create_tok= get_next_token(count_next_tok_local);
							switch (create_tok)
							{
							case C:
								cout << "Read ')' successful" << endl;
								create_tok = get_next_token(count_next_tok_local);
								switch (create_tok)
								{
								case END:
									cout << "Read ';' successful" << endl;
									return 1;
								default:
									cout << "Error, incorrect ';' entry!" << endl;
									return 0;
								}
							default:
								cout << "Error, incorrect ')' entry!" << endl;
								return 0;
							}
						}
						else return 0;
						
					}
				default:
					cout << "Error, incorrect '(' entry!" << endl;
					return 0;
				}
			default:
				cout << "Error, incorrect 'ID' entry!" << endl;
				return 0;
			}
		default:
			cout << "Error, incorrect 'table' entry!" << endl;
			return 0;
		}
		
	default:
		cout << "Error, incorrect 'create' entry!" << endl;
		return 0;
	}

}
int DROP_STM(tok_names tok, int& count_next_tok_local)
{
	if(count_next_tok_local<=1)count_next_tok_local += 1;
	tok_names drop_tok = tok;
	switch (drop_tok)
	{
	case drop:
		cout << "Read 'drop' successful" << endl;
		drop_tok = get_next_token(count_next_tok_local);
		switch (drop_tok)
		{
		case table:
			cout << "Read 'table' successful" << endl;
			drop_tok = get_next_token(count_next_tok_local);
			switch (drop_tok) {
			case ID:
				cout << "Read 'id' successful" << endl;
				drop_tok = get_next_token(count_next_tok_local);
				switch (drop_tok) {
				case END:
					cout << "Read ';' successful" << endl;
					return 1;
					break;
				default: 
					cout << "Error, incorrect 'id' entry!" << endl;
					return 0;
				}
			default:
				cout << "Error, incorrect ';' entry!" << endl;
				return 0;
			}
		default:
			cout << "Error, incorrect 'table' entry!" << endl;
			return 0;
		}
	}
	return 1;


}
int ALTER_STM(tok_names tok,int &count_next_tok_local)
{
	if (count_next_tok_local <= 1)count_next_tok_local += 1;
	tok_names ALTER_tok = tok;
	switch (ALTER_tok)
	{
	case alter:
		cout << "Read 'alter' successful" << endl;
		ALTER_tok = get_next_token(count_next_tok_local);
		switch (ALTER_tok)
		{
		case table:
			cout << "Read 'table' successful" << endl;
			ALTER_tok = get_next_token(count_next_tok_local);
			switch (ALTER_tok)
			{
			case ID:
				cout << "Read 'id' successful" << endl;
				ALTER_tok = get_next_token(count_next_tok_local);
				switch (ALTER_tok)
				{
				case add:
					cout << "Read 'add' successful" << endl;
					ALTER_tok = get_next_token(count_next_tok_local);
					switch (ALTER_tok)
					{
					case column:
						cout << "Read 'column' successful" << endl;
						ALTER_tok = get_next_token(count_next_tok_local);
						switch (ALTER_tok)
						{
						case ID:
							if (FIELD_DEF_LIST(ALTER_tok, count_next_tok_local))
							{
								ALTER_tok = get_next_token(count_next_tok_local);
								switch (ALTER_tok)
								{
								case END:
									cout << "Read ';' successful" << endl;
									return 1;
								default:
									cout << "Error, incorrect ';' entry!" << endl;
									return 0;
								}
							}
							else
							{
								return 0;
							}
							
						}
					default:
						cout << "Error, incorrect 'column' entry!" << endl;
						return 0;
					}
				case drop:
					cout << "Read 'drop' successful" << endl;
					ALTER_tok = get_next_token(count_next_tok_local);
					switch (ALTER_tok)
					{
					case column:
						cout << "Read 'column' successful" << endl;
						ALTER_tok = get_next_token(count_next_tok_local);
						switch (ALTER_tok)
						{
						case ID:
							cout << "Read 'id' successful" << endl;
							ALTER_tok = get_next_token(count_next_tok_local);
							switch (ALTER_tok)
							{
							case END:
								cout << "Read ';' successful" << endl;
								return 1;
							default:
								cout << "Error, incorrect ';' entry!" << endl;
								return 0;
							}
						default:
							cout << "Error, incorrect 'id' entry!" << endl;
							return 0;
						}
					default:
						cout << "Error, incorrect 'column' entry!" << endl;
						return 0;
					}
				default:
					cout << "Error, incorrect 'add or drop' entry!" << endl;
					return 0;
				}
			default:
				cout << "Error, incorrect 'id' entry!" << endl;
				return 0;
			}
		default:
			cout << "Error, incorrect 'table' entry!" << endl;
			return 0;
		}
	default:
		cout << "Error, incorrect 'alter' entry!" << endl;
		return 0;
	}
}
int STAT(tok_names tok,int count_next_tok_local)
{
	switch (tok)
	{
	case create:
		if (CREATE_STM(tok, count_next_tok_local))
		{
			tok = get_next_token(count_next_tok_local);
			if (tok == ENDLINE)
			{
				cout << endl << "The CREATE is syntactically correct!!!" << endl;
				return 1;
			}
			else
			{
				STAT(tok, count_next_tok_local);
				cout << endl << "The CREATE is syntactically correct!!!" << endl;
				return 1;
			}

		}else
		{
			cout << "The CREATE is syntactically incorrect" << endl;
			return 0;
		}

	case drop:
		if (DROP_STM(tok, count_next_tok_local))
		{
			tok = get_next_token(count_next_tok_local);
			if (tok == ENDLINE)
			{
				cout << endl << "The DROP is syntactically correct!!!" << endl;
				return 1;
			}
			else
			{
				STAT(tok, count_next_tok_local);
				cout << endl << "The DROP is syntactically correct!!!" << endl;
				return 1;
			}

		}else
		{
			cout << "The DROP is syntactically incorrect" << endl;
			return 0;
		}

			case alter:
				if(ALTER_STM(tok, count_next_tok_local))
				{
					tok = get_next_token(count_next_tok_local);
					if (tok == ENDLINE)
					{
						cout << endl << "The ALTER is syntactically correct!!!" << endl;
						return 1;
					}
					else
					{
						STAT(tok, count_next_tok_local);
						cout << endl << "The ALTER is syntactically correct!!!" << endl;
						return 1;
					}
					
				}else 
				{
					cout << "The ALTER is syntactically incorrect" << endl;
					return 0;
				}
	default:
		cout << "Error, incorrect <STAT> entry!" << endl;
		return 0;
	}
	return 1;
}
void parse_input()
{
	tok_names tok;
	int count = 0;
	tok = get_next_token(count);
	int count_next_tok_local = 0;
	STAT(tok, count_next_tok_local);
}

int main()
{
	if (!lexer("exCode.txt"))
	{
		cout << "Error entering the command, make sure you wrote it correctly!!!" << endl;
		return 0;
	}
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < lexem.size(); i++)
	{
		if (lexem[i] == "create" || lexem[i] == "drop" || lexem[i] == "alter" || lexem[i] == "table" || lexem[i] == "column" || lexem[i] == "add" || lexem[i] == "not" || lexem[i] == "null"|| lexem[i] == "("|| lexem[i] == ")"|| lexem[i] == ";" || lexem[i] == ",")
		{
			token[i] = lexem[i];
		}
	}
	vector <string> token_2;
	for (int i = 0; i < token.size(); i++)
	{
		if (token[i] == "DLM") continue;
		else token_2.push_back(token[i]);
	}
	token.clear();
	token = token_2;
	token.push_back("-|");
	cout << "Input line: " << endl;
	for (int i = 0; i < lexem.size(); i++)
	{
		cout << lexem[i] << "|";
		if (lexem[i] == ";") cout << endl;
	}

	cout << endl;
	cout <<endl<< "line through lex. analyzer: " << endl;
	for (int i = 0; i < token.size(); i++)
	{
		cout << token[i] << "|";
		if (token[i] == ";") cout << endl;
	}
	cout << endl << endl;
	parse_input();

	return 0;
}
