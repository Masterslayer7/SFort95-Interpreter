#include "parserInterp.h"
#include <stack> 

//Keeps record of defined variables, key is variable name, value is a bool set to true when var is first defined
map<string, bool> defVar;
map<string, Token> SymTable;

map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

string length;
bool stringInit;
int ifNestLevel = 0;
LexItem varType;


namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}
static int error_count = 0;

int ErrCount()
{
    return error_count;
}

// function to return an error
void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}


//Prog ::= PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line) {
	LexItem t;
	bool hasError = false;

	//DEBUG PURPOSES 
	/*
	while(t != END){
		t = Parser::GetNextToken(in, line);
		cout << t << endl;
	}
	*/
	//Checks for Program
	t = Parser::GetNextToken(in, line);
 	if( t != PROGRAM ) { 
		ParseError(line, "Missing PROGRAM");
		return false;
	}

	//Checks for program Name
	t = Parser::GetNextToken(in, line);
	if(t != IDENT) {
		ParseError(line, "Missing Program name");
		return false;
	}

	//Decl Loop
    while(!hasError){
        t = Parser::GetNextToken(in, line);
        if (t == INTEGER || t == REAL || t == CHARACTER){
            Parser::PushBackToken(t);
			bool decl = Decl(in, line); 
            //checks if decl had any errors
            if (!decl){ 
			    return false;
		    }
		}
        else{
			Parser::PushBackToken(t);
            break;
        }
	}

	//STATEMENT LOOP
	while(!hasError){
        t = Parser::GetNextToken(in, line);
        if(t != END && t != PROGRAM){
            Parser::PushBackToken(t);
			bool stmt = Stmt(in, line);
            if (!stmt){
			    hasError = true;
		    }
		}
        else{
            break;
        }
	}

    if (!hasError){
        //Checks if the program has an end
        if( t != END ) {
            ParseError(line, "Missing END ");
            Parser::PushBackToken(t);
            hasError = true;
        }

        //Checks if the Program reserved token in there
        t = Parser::GetNextToken(in, line);
        if(t != PROGRAM) {
            ParseError(line, "Missing PROGRAM");
            Parser::PushBackToken(t);
            hasError = true;
        }

        //Checks if the program has a name
        t = Parser::GetNextToken(in, line);
        if(t != IDENT) {
            ParseError(line, "Missing Program name");
            Parser::PushBackToken(t);
            hasError = true;
        }
    }

	//returns done and true if code had no errors
	if(!hasError){
		cout << "(DONE)" << endl;
		return true;
	}
	return false;
}

//Decl ::= Type :: VarList 
bool Decl(istream& in, int& line) {
	//resets stringInit and lenght
	stringInit = false;
	length = "1";
	int strlen = 0;
	LexItem t;
	LexItem idtok;


	//checks if type
	bool type = Type(in, line);
 	if(!type) {
		ParseError(line, "Incorrect Declaration in Program1111");
		return false;
	}

	//checks for double colon
	t = Parser::GetNextToken(in, line);
	if(t != DCOLON) {
		ParseError(line, "Missing Double Colons");
		return false;
	}

	strlen = stoi(length);
	idtok = varType;
	bool varList = VarList(in, line, idtok, strlen);
	if(!varList) {
		ParseError(line, "Missing Variables List.");
		return false;
	}

	//Handels the output of definition of string using global varibles 
	if(stringInit){
		// cout << "Definition of Strings with length of " << length <<  " in declaration statement." << endl;
	}
	return true;
}

//Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)]
 bool Type(istream& in, int& line){
	LexItem t;
	string Length;
	t = Parser::GetNextToken(in, line);

	if(t == INTEGER || t == REAL){
		// DEBUG cout << "I saw real or integer1" << endl; //---------------------------------------------------------------------------
		varType = t;
		return true;
	}
	else if(t == CHARACTER){
		// DEBUG cout << "Character was found" << endl; //---------------------------------------------------------------------------
		varType = t;
		t = Parser::GetNextToken(in, line);
		if(t == LPAREN){

			t = Parser::GetNextToken(in, line);
			if(t == LEN){

				t = Parser::GetNextToken(in, line);
				if(t == ASSOP){

					t = Parser::GetNextToken(in, line);
					length = t.GetLexeme();
					if(t == ICONST){

						t = Parser::GetNextToken(in, line);
						if(t == RPAREN){
							stringInit = true;
							return true;
						}
						return false;
					}
					return false;
				}
				return false;
			}
			return false;
		}
		Parser::PushBackToken(t);
		return true;
	}
	return false;
 }

// VarList ::= Var [= Expr] {, Var [= Expr]}
bool VarList(istream& in, int& line, LexItem & idtok, int strlen){
	LexItem t;
	bool done = false;
	string emptystring = "";
	string VarName;
	Value retVal;
	LexItem varType;
	varType = idtok;

	//checks for var 
	bool var = Var(in, line, idtok);
	if (!var){
		ParseError(line, "Missing Variable Name");
		return false;
	}
	else{
		//uses getNextToken because Var pushs back a token so that varname can be accsessd 
		VarName = idtok.GetLexeme();

	}

	//checks for assignmnet operator
	t = Parser::GetNextToken(in, line);
	if (t == ASSOP){
		//checks for expression 
		bool expr = Expr(in, line, retVal);
		if(!expr){
			ParseError(line, "No expression in assignment");
			return false;
		}

		//adds var and expr to tempsresult
		//gives retval a len
		if(varType.GetToken() == CHARACTER){
			retVal.SetstrLen(strlen);
			cout << "variable added tempsresult: " << VarName << "with strlen of: " << to_string(TempsResults[VarName].GetstrLen()) << endl; //debug
		}
		TempsResults[VarName] = retVal;
		//adds var and type to symtable
		SymTable[VarName] = varType.GetToken();

		

		//Check for variable redefinition
		if (defVar.find(VarName) == defVar.end()){
			defVar[VarName] = true;
			// cout << "Initialization of the variable " << VarName << " in the declaration statement." << endl;
		}
		else{
			ParseError(line, "Variable redefinition");
			return false;
		}
	}
	else{
		//if its string and no assignmnet
		if(varType.GetToken() == CHARACTER){
			TempsResults[VarName] = Value(emptystring);
			TempsResults[VarName].SetstrLen(strlen);
			cout << "variable added tempsresult: " << VarName << "with strlen of: " << to_string(TempsResults[VarName].GetstrLen()) << endl; //debug
		}
		else{
			//assigns varname to error value
			TempsResults[VarName] = Value();
		}
		
		
		//adds var to symtable with type
		SymTable[VarName] = varType.GetToken();

		//check for variable redefiniton
		if (defVar.find(VarName) == defVar.end()){
			defVar[VarName] = false;
		}
		else{
			ParseError(line, "Variable redefinition");
			return false;
		}
		Parser::PushBackToken(t);
	}

	// DEBUG cout << "I Got here in VarList, before while loop with TOKEN: " << t << endl;
	while (!done)
	{
		t = Parser::GetNextToken(in, line);
		// DEBUG cout << "TOken that i am checking for comma " << t << endl; //--------------------------------------------------------------------------------------------
        //if there is a comma
		if (t == COMMA){
            //check for var
            bool var = Var(in, line, idtok);
            if (!var){
                ParseError(line, "Missing Variable Name");
                return false;
            }
			else{
				VarName = idtok.GetLexeme();
				defVar[VarName] = false;
				if(varType.GetToken() == CHARACTER){
					TempsResults[VarName] = Value(emptystring);
					TempsResults[VarName].SetstrLen(strlen);
					cout << "variable added tempsresult: " << VarName << "with strlen of: " << to_string(TempsResults[VarName].GetstrLen()) << endl; //debug
				}	
				else{
					TempsResults[VarName] = Value();
				}
				SymTable[VarName] = varType.GetToken();
				// cout << "variable added tempsvariable in loop: " << VarName << "with value: " << Value() << endl;//debug
			}		
 
            //checks for assignmnet operator
			t = Parser::GetNextToken(in, line);
			if (t == ASSOP){

				//checks for expression 
				bool expr = Expr(in, line, retVal);
				if(!expr){
					ParseError(line, "No expression in assignment");
					return false;
				} 

				//puts var and expr in maps
				defVar[VarName] = true;
				if(varType.GetToken() == CHARACTER){
					retVal.SetstrLen(strlen);
				}	
				TempsResults[VarName] = retVal;
				SymTable[VarName] = varType.GetToken();
				cout << "variable added tempsvariable in loop: " << VarName << "with value: " << retVal << endl; //debug
				// cout << "Initialization of the variable " << VarName << " in the declaration statement." << endl;

			}
			// If its not = then put back because its most likely comma
			else{
				Parser::PushBackToken(t);
			}
		}
		//returns true if no more varibles to initialize
		else{
			// DEBUG cout << "i found something that not a variable list, exiting variable list" << endl; //------------------------------
			Parser::PushBackToken(t);
			return true;
		}
	}
	return var;
 }

// Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream& in, int& line){
	LexItem t;
	t = Parser::GetNextToken(in, line);
	// cout << "Token in Stmt is " << t << endl; //debug ------------------------------------------------------------------
	// checks if next token is print
	if (t == PRINT){
		bool printstmt = PrintStmt(in, line);
		if (!printstmt){
            ParseError(line, "Incorrect Statement in program");
			return false;
		}
        return printstmt;
	}
	//checks if next Token is a variable
	else if (t == IDENT){
        Parser::PushBackToken(t);
		bool assignstmt = AssignStmt(in, line);
		if (!assignstmt){
            ParseError(line, "Incorrect Statement in program");
			return false;
		}
        return assignstmt;
	}
	//Checks if the next token is a IF
	else if (t == IF){
		bool blockifStatement = BlockIfStmt(in, line);
		if (!blockifStatement){
            ParseError(line, "Incorrect Statement in blockIfstatement");
			return false;
		}
        return blockifStatement;
	}
	else if(t == ELSE){
		ParseError(line, "Missing THEN in IF statement");
		return false;
	}
	return false;
}

//PrintStmt:= PRINT *, ExpreList 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;

	t = Parser::GetNextToken(in, line);
	
 	if( t != DEF ) {
		
		ParseError(line, "Print statement syntax error.");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	
	if( t != COMMA ) {
		
		ParseError(line, "Missing Comma.");
		return false;
	}
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}

	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;

	return ex;
}//End of PrintStmt

//BlockIfStmt ::= IF (RelExpr) THEN {Stmt} [ELSE {Stmt}] END IF
bool BlockIfStmt(istream& in, int& line){
	//Variables 
    LexItem t;
    int ElseUsed = 0;
	bool realExprBool = false; 
	Value retVal;

	t = Parser::GetNextToken(in, line);
	// DEBUG cout << "TOKEN in block if statement is " << t << endl; // -----------------------------------------------------
	if(t == LPAREN){
		bool relExpr = RelExpr(in, line, retVal);
    	if (!relExpr){
            ParseError(line, "No real Expression");
			return false;
    	}

		//checks if the relexpr is true of false
		if(retVal.IsBool()){
			if(retVal.GetBool()){
				realExprBool = true;
			}
		}
		else{
			ParseError(t.GetLinenum(), "Run-Time Error-Illegal Type for If statement condition");
			return false;
		}

		//checks for right parenthesis before continuing
		t = Parser::GetNextToken(in, line);
		// DEBUG cout << "TOKEN after relex " << t << endl; // ----------------------------
		if(t != RPAREN){
			ParseError(line, "Missing right paranthesis");
			return false;
		}
		//Need to go to next token if token is RPAREN
		t = Parser::GetNextToken(in, line);
	}
	
	// DEBUG cout << "Token when checking for then: " << t << endl; //-------------------------------------------------------------
    if (t == THEN)
    {
		if(!realExprBool){
			t = Parser::GetNextToken(in, line);
			while (t != ELSE)
			{
				t = Parser::GetNextToken(in, line);
			}
			ElseUsed++;
		}

		ifNestLevel++;
        while (ElseUsed <= 1){
            bool stmt = Stmt(in, line);
			t = Parser::GetNextToken(in, line);
            if (!stmt){
			    return false;
            }
            if (t == ELSE){
                ElseUsed++;
            }
            else if(t == END){
                t = Parser::GetNextToken(in, line);
                if (t == IF){
					// cout << "End of Block If statement at nesting level " << to_string(ifNestLevel) << endl;
					ifNestLevel--;
                    return true;
                }
                ParseError(line, "Missing IF at End of IF statement");
                return false;
            }
			else{
				Parser::PushBackToken(t);
			}
        }
        
    }
    else
    {
		//need to push back the token because it may be a part of the stmt in the if
		Parser::PushBackToken(t);
        bool simpleIfstmt = SimpleIfStmt(in, line);
        if (!simpleIfstmt){
            ParseError(line, "Missing THEN Statement");
			return false;
		}
			
    }
	return true;
}

//SimpleIfStmt ::= IF (RelExpr) SimpleStmt
bool SimpleIfStmt(istream& in, int& line){
    bool simpleStmt = SimpleStmt(in, line);
    if (!simpleStmt){
        return false;
    }
	return simpleStmt;
}
//SimpleStmt ::= AssigStmt | PrintStmt
bool SimpleStmt(istream& in, int& line){
    LexItem t;
	t = Parser::GetNextToken(in, line);
	// DEBUG cout<< "Token in simplestatement " << t << endl; //-----------------------------------------------------------
    if (t == PRINT){
		bool printstmt = PrintStmt(in, line);
		if (!printstmt){
            ParseError(line, "Incorrect Statement in program");
			return false;
		}
		//correct output
		cout << "Print statement in a Simple If statement." << endl;
        return printstmt;
	}
	//checks if next Token is a variable
	else if (t == IDENT){
		Parser::PushBackToken(t);
		bool assignstmt = AssignStmt(in, line);
		if (!assignstmt){
            ParseError(line, "Incorrect Statement in program");
			return false;
		}
		cout << "Assignmnet statement in a Simple If statement." << endl;
        return assignstmt;
	}
	return false;
}

// AssignStmt ::= Var = Expr
bool AssignStmt(istream& in, int& line){
    LexItem t;
	int templen = 1;
	Value retVal;
	LexItem idtok;
	
	bool var = Var(in, line, idtok);
	if(!var){
		ParseError(line, "No expression in assignment");
		return false;
	}

	//checks for assignmnet operator
	t = Parser::GetNextToken(in, line);

	if (t == ASSOP){
		//checks for expression 
		bool expr = Expr(in, line, retVal);
		if(!expr){
			ParseError(line, "No expression in assignment");
			return false;
		}
		//test11 error (string reassigned to int or real)
		if(TempsResults[idtok.GetLexeme()].GetType() == 2){
			if(retVal.GetType() != 2){
				ParseError(t.GetLinenum(), "Illegal mixed-mode assignment operation");
				return false;
			}
		}
		// DEBUG cout << "The token after finishing assignSTMT: " << t << endl; //--------------------------------
		if(TempsResults[idtok.GetLexeme()].GetType() == 2){
			templen = TempsResults[idtok.GetLexeme()].GetstrLen();
			retVal.SetstrLen(templen);
		}
		TempsResults[idtok.GetLexeme()] = retVal;
        return expr;
	}
    else{
        ParseError(line, "Missing Assignment Operator");
        return false;
    }
}

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;

	status = Expr(in, line, retVal);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	
	ValQue->push(retVal);
	while (true)
	{
		LexItem tok = Parser::GetNextToken(in, line);
		// DEBUG cout << "TOKEN In expr list checking for comma: " << tok << endl;
		if (tok == COMMA) {
			status = Expr(in, line, retVal);
			if(!status){
				ParseError(line, "Missing Expression");
				return false;
			}
			ValQue->push(retVal);
		}

		else if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
		else{
			Parser::PushBackToken(tok);
			return true;
		}
	}
	return status;
}//End of ExprList

//RelExpr ::= Expr [ ( == | < | > ) Expr ]
bool RelExpr(istream& in, int& line, Value & retVal){
    LexItem t;
	Value finalValue;

    bool expr = Expr(in, line, retVal);
    if(!expr){
        ParseError(line, "No expression in assignment");
        return false;
    }
	finalValue = retVal;
    
	t = Parser::GetNextToken(in, line);
	if(t == EQ || t == LTHAN || t == GTHAN){
		bool expr = Expr(in, line, retVal);
		if(!expr){
			ParseError(line, "No expression in relexpr");
			return false;
		}
		switch (t.GetToken())
			{ 
			case EQ:
				finalValue = finalValue == retVal;
				break;

			case LTHAN:
				finalValue = finalValue > retVal;
				break;

			case GTHAN:
				finalValue = finalValue < retVal;
				break;

			default:
				break;
			}

		if(finalValue.IsErr()){
			ParseError(t.GetLinenum(), "Using uninitialized Variable");
			return false;
		}

		retVal = finalValue;
		return expr;
	}
	//will return expr from the first iteration if expr from the second is never called
	return expr;
}

//Expr ::= MultExpr { ( + | - | // ) MultExpr }

bool Expr(istream& in, int& line, Value & retVal){
    LexItem t;
	Value finalValue;

    bool multexpr = MultExpr(in, line, retVal);
    if(!multexpr){
        ParseError(line, "No expression in assignment");
        return false;
    }
    
	cout<<"retval after multexpr in expr is: " << retVal << endl; //debug
	finalValue = retVal;
	
    while(true){
		t = Parser::GetNextToken(in, line);
		// DEBUG cout << "While checking for expr " << t << endl;
        if(t == PLUS || t == MINUS || t == CAT){
			// since there is a operator check for a expression
			bool multexpr = MultExpr(in, line, retVal);
			if(!multexpr){
				ParseError(line, "No expression in multexpr");
				return false;
			}
			switch (t.GetToken())
			{ 
			case PLUS:
				finalValue = finalValue + retVal;
				break;

			case MINUS:
				finalValue = finalValue - retVal;
				break;

			case CAT:
				finalValue = finalValue.Catenate(retVal);
				break;

			default:
				break;
			}
			if(finalValue.IsErr()){
				ParseError(line, "Using uninitialized Variable");
				return false;
			}
        }
		else{
			retVal = finalValue;
			Parser::PushBackToken(t);
			return true;
		}
    }
    return multexpr;
}

//MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream& in, int& line, Value & retVal){
	LexItem t;
	Value finalValue;

    bool termExpr = TermExpr(in, line, retVal);
    if(!termExpr){
        ParseError(line, "No expression in assignment");
        return false;
    }
	cout<<"retval after termexpr in multexpr is: " << retVal << endl; //debug
	finalValue = retVal;

    while(true){
		t = Parser::GetNextToken(in, line);
        if(t == MULT || t == DIV){
			// since there is a operator check for a expression
			bool termExpr = TermExpr(in, line, retVal);
			if(!termExpr){
				ParseError(line, "No expression in multexpr");
				return false;
			}

			switch (t.GetToken())
			{ 
			case MULT:
				finalValue = finalValue * retVal;
				break;

			case DIV:
				if(retVal.IsInt()){
					if(retVal.GetInt() == 0){
						ParseError(t.GetLinenum(), "cannot divide by zero");
						return false;
					}
				}
				else if(retVal.IsReal()){
					if(retVal.GetReal() == 0){
						ParseError(t.GetLinenum(), "cannot divide by zero");
						return false;
					}
				}
				finalValue = finalValue / retVal;
				break;

			default:
				break;
			}
			//checks if its error
			if(finalValue.IsErr()){
				ParseError(line, "Using uninitialized Variable");
				return false;
			}
			
        }
		else{
			retVal = finalValue;
			Parser::PushBackToken(t);
			return true;
		}
    }
    return termExpr;
}

// TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream& in, int& line, Value & retVal){
	LexItem t;
	stack<Value> powStack; 

    bool sfactor = SFactor(in, line, retVal);
    if(!sfactor){
        ParseError(line, "No sfactor in assignment");
        return false;
    }
	cout<<"retval after sfactor in termexpr is:" << retVal << "n" << endl; //debug
	//push retval onto queue to potentially be
    powStack.push(retVal);

    while(true){
		t = Parser::GetNextToken(in, line);
		// DEBUG cout << "Token when checking for ** " << t << endl; //---------------
        if(t == POW){
			// since there is a operator check for a expression
			bool sfactor = SFactor(in, line, retVal);
			if(!sfactor){
				ParseError(line, "No sfactor after ** ");
				return false;
			}
			powStack.push(retVal);
        }
		else{
			//solves the stack to turn a b c to a^(b^c)
			retVal = powStack.top();
			powStack.pop();
			while (!(powStack.empty())) {
				Value base = powStack.top();
				retVal = base.Power(retVal);
				powStack.pop();
			}
			Parser::PushBackToken(t);
			return true;
		}
    }
    return sfactor;
}

// SFactor ::= [+ | -] Factor
bool SFactor(istream& in, int& line, Value & retVal){
	LexItem t;
	t = Parser::GetNextToken(in, line);

	if(t == PLUS){
		bool factor = Factor(in, line, 1, retVal);
		if(!factor){
			ParseError(line, "No Factor in Sfactor");
			return false;
		}
		return factor;
	}
	else if(t == MINUS){
		bool factor = Factor(in, line, -1, retVal);
		if(!factor){
			ParseError(line, "No Factor in Sfactor");
			return false;
		}
		return factor;
	}
	else{
		Parser::PushBackToken(t);
		bool factor = Factor(in, line, 1, retVal);
		if(!factor){
			ParseError(line, "No Factor in Sfactor");
			return false;
		}
		return factor;
	}
	return false;
}

// Var ::= IDENT
bool Var(istream& in, int& line, LexItem & idtok){
	LexItem t;
	t = Parser::GetNextToken(in, line);
	// DEBUG  cout << "What i am checking for var is: " << t << endl; // -----------------------------------------------------------------
	if (t == IDENT){
		idtok = t;
		return true;
	}
	return false;
}

// Factor ::= IDENT | ICONST | RCONST | SCONST | (Expr)
bool Factor(istream& in, int& line, int sign, Value & retVal){
	//variables
	LexItem t;
	t = Parser::GetNextToken(in, line);
	//found an ident means you found a variable
	if (t == IDENT){
		//checks if a sign was used on a string
		if(defVar.find(t.GetLexeme()) != defVar.end()){
			//checks for type
			if(SymTable[t.GetLexeme()] == CHARACTER){
				if(sign == -1){
					ParseError(t.GetLinenum(), "Illegal Operand Type for Sign Operator");
					return false;
				}		
				//checks if len is more or less then initialized
				cout << to_string(TempsResults[t.GetLexeme()].GetstrLen()) << endl; //debug
				if(TempsResults[t.GetLexeme()].GetstrLen() > TempsResults[t.GetLexeme()].GetString().length()){
					//adds whitespaces
					while (TempsResults[t.GetLexeme()].GetstrLen() != TempsResults[t.GetLexeme()].GetString().length()){
						TempsResults[t.GetLexeme()].SetString(TempsResults[t.GetLexeme()].GetString() + " ");
					}
				}
				else if(TempsResults[t.GetLexeme()].GetstrLen() < TempsResults[t.GetLexeme()].GetString().length()){
					//cuts characters
					TempsResults[t.GetLexeme()].SetString(TempsResults[t.GetLexeme()].GetString().substr(0, TempsResults[t.GetLexeme()].GetstrLen()));
				}
			}
			else if(SymTable[t.GetLexeme()] == REAL){
				if(TempsResults[t.GetLexeme()].IsInt()){
					TempsResults[t.GetLexeme()] = Value((double)TempsResults[t.GetLexeme()].GetInt() * sign);
				}
			}
			else{
				TempsResults[t.GetLexeme()] = Value(TempsResults[t.GetLexeme()] * sign);
			}
            retVal = Value(TempsResults[t.GetLexeme()]);
			return true;
			
		}
		ParseError(line, "Using Undefined Variable");
		return false;
	}
	else if(t == ICONST || t == RCONST || t == SCONST){
		//sets retval to lexeme
		switch (t.GetToken()) {
		case (ICONST):
			retVal = Value(stoi(t.GetLexeme()) * sign);
			break;

		case (RCONST):
			retVal = Value(stod(t.GetLexeme()) * sign);
			break;
		
		case (SCONST):
			if(sign == -1){
				ParseError(t.GetLinenum(), "Illegal Operand Type for Sign Operator");
				return false;
			}
			retVal= Value(t.GetLexeme());
			break;
		
		default:
			break;
		}
		return true;
	}
	else if(t == LPAREN){
		bool expr = Expr(in, line, retVal);
		if(!expr){
			ParseError(line, "No Expression in factor");
			return false;
		}
		t = Parser::GetNextToken(in, line);
		if(t == RPAREN){
			return expr;
		}
		ParseError(line, "Missing right paranthesis");
		return false;
	}
	return false;
}//blank line to get to 1000
