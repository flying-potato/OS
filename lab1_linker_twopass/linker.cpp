#define MACHINE_SIZE (512)
#define MAX_SYMBOL_LENGTH (16)
#define MAX_DEF_LIST_LENGTH (16)
#define MAX_USE_LIST_LENGTH (16)
enum ErrorCode { 
    NUM_EXPECTED,              // Number expect 
    SYM_EXPECTED,              // Symbol Expected 
    ADDR_EXPECTED,             // Addressing Expected which is A/E/I/R   
    SYM_TOO_LONG,              // Symbol Name is too long
    TO_MANY_DEF_IN_MODULE,     // > 16  
    TO_MANY_USE_IN_MODULE,     // > 16 
    TO_MANY_INSTR
}; 
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <iomanip>
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <list>
#include <map>
#include <set>


using namespace std;
//----------specification-----------
void init();
void readLine();
void readWord();
void readStr(string &str);
bool tellNum(char c);
void readInt(int &val);
void readInstr(int &instr);
void readChar(char &c );
void readDefList();
void readUseList();
void readInstrList();

void getTotalInstr();

void printSymTable();
void printmoduleBaseAddr();

int linenum , charoffset, wordlen;
int defcount, usecount , codecount; 
int modulenum=0, totaldefcount=0, baseAddr = 0; int totalmodulenum ,acc, total_instr = 0;
char type; int instr;

int mem_num = 0;


ifstream infile;
stringstream line;
stringstream nextWord;

map<int, int> moduleBaseAddr; //can be stored
std::map<int, int> moduleSize;
map<string, int> symAddr; //can be stored
map<int, string> addrSym; 

map<string, int> defList;
vector<string> useList;
set<string> alluseList; //alluseList.insert()
map<string, bool> ifUsed_uselist;
map<string, bool> ifUsed_deflist; //contains every module's unused symbol

set<string> multiDefSym;
map<string, int> symInModule;


struct ParseError: public exception
{
  ParseError(ErrorCode errcode, int linenum, int lineoffset)
  {  
    this-> errcode = errcode;
    this-> linenum = linenum; 
    this-> lineoffset = lineoffset;
  }

  const string what(){
    static string errstr[] ={ 
        "NUM_EXPECTED", 
        "SYM_EXPECTED",           
        "ADDR_EXPECTED",             
        "SYM_TOO_LONG",              
        "TO_MANY_DEF_IN_MODULE",    
        "TO_MANY_USE_IN_MODULE",    
        "TO_MANY_INSTR"           
    };   
    char * x = new char[80];
    sprintf(x, "Parse Error line %d offset %d: %s\n", this->linenum, this->lineoffset, errstr[this->errcode].c_str() );
    return x ;
  }
  private:
    ErrorCode errcode;
    int linenum, lineoffset;
};


int main( int argc, char* argv[] ){

  init();
  symAddr.clear();
  infile.open(argv[1]);

  //-----------------first pass starts-------------------------------------------
  try{

    while(infile){
      readDefList();
      readUseList();

      // do not parse code and address
      readInt(codecount); 
      total_instr += codecount;
      if(total_instr> MACHINE_SIZE){
        throw  ParseError( TO_MANY_INSTR, linenum, charoffset );
      }
      moduleSize[modulenum] = codecount; //get moduleSize
      baseAddr += codecount; //update the curBaseAddr
      for(std::map<string, int>::iterator it = defList.begin(); it != defList.end(); it++ ){
        string varname = it->first;
        int rel = it->second; 

        if(rel > codecount && varname.length()>0){
          const char * warnmsg = "Warning: Module %d: %s too big %d (max=%d) assume zero relative\n" ;
          printf(warnmsg, modulenum, varname.c_str(), rel, codecount-1);
          //update defList and symAddr
          defList[varname] = 0;
          symAddr[varname] = moduleBaseAddr[modulenum] ;
        }
      }

      for(int i= 0 ; i<codecount; i++){
        
        readChar(type); //how to throw a 
        readInstr(instr);
      }

    }
    infile.close();
    printSymTable();

    moduleBaseAddr.erase(modulenum);
    // printmoduleBaseAddr();

    totalmodulenum = moduleBaseAddr.size();

  }catch(ParseError  & e){
    // cout<<"linenum: "<<linenum<<"offset: "<<charoffset<<endl;  
    cout<<e.what()<<endl;
    infile.close();
    exit(1) ;
  }


  

  //-----------------first pass ends--------------
  cout<<endl;
  //=================second pass starts==========================================
  cout <<"Memory Map"<<endl;
  init();

  infile.open(argv[1]);
  map<int,int>::iterator module_ptr = moduleBaseAddr.begin();
  for(   ;   module_ptr != moduleBaseAddr.end(); module_ptr++ )
  {
    // pass2: def
    modulenum = module_ptr->first ;
    baseAddr += module_ptr->second;
    readInt(defcount) ; 
    string var;
    int rel;
    for(int i = 0; i<defcount; i++){
      readStr(var);
      readInt(rel) ; 

    }
    // pass2 :use
    readUseList();

    // pass2: code
    readInstrList();

  }

  //================ENDS================
  //print syms defined but not used

  for (std::map<string,int>::iterator it=symAddr.begin(); it!=symAddr.end(); ++it){
    // cout<<it->first <<" - " <<it->second<<endl;
    string varname = it->first;
    if(varname.length() > 0 && alluseList.count(varname) == 0){
      std::printf("Warning: Module %d: %s was defined but never used\n", symInModule[varname] ,varname.c_str());
    }
  }
  infile.close();

  return 1;
}



void init(){
  line.clear();
  linenum = 0;
  wordlen = 0;
  baseAddr = 0;
  modulenum = 0;
  mem_num = 0 ;
}
void readLine() { //read a new line
  string strline;
  if( getline(infile, strline)) {
    line.clear(); 
    line << strline;
    linenum++; //linenum starts from 1
    charoffset = 1; // char offset starts from 1
  }
}

void readWord() {
  string str;

  charoffset += wordlen; //plus the last word's length

  while( !infile.eof() ) {

    //jump white space to get nextword
    char ch = line.peek() ;
    while( ch == '\t' || ch == ' ' ) {
      line.ignore(1);
      charoffset ++;
      ch = line.peek();
    }
    //when initilized , line is empty ,so must fail then readLine()
    //line>>str get the first word
    if(!(line>>str)){ //when meet the \n, line>>str return false
      readLine();
      continue;
    }
    nextWord.flush();
    nextWord.clear();
    // nextWord << ' ' << str; //a string in nextWord
    nextWord<<str;
    wordlen = str.length();
    //when get the empty line, then we go to eof, jump out loop
    return ;
  }
  return ;
}

void readStr(string &str){
  //get a string from nextWord
  readWord();
  // int testint;
  // if(nextWord>>testint){throw ParseError( SYM_EXPECTED, linenum, charoffset );}
  if(!(nextWord >> str)||tellNum(str[0]) ){
    if(!infile.eof()){
    // if(!infile.eof() || symAddr.empty()){
      throw ParseError( SYM_EXPECTED, linenum, charoffset );
    }
    if(infile.eof() && symAddr.size()<defcount)
      throw ParseError( SYM_EXPECTED, linenum, charoffset );
  } 
  if(str.length()>16){
    if(!infile.eof()){
      throw ParseError( SYM_TOO_LONG, linenum, charoffset );

    }
  }
  return ;
}
bool tellNum(char c){

  for(int i = 48;i<=57 ;i++){
    char cc = i;
    if(c == cc){return true;}
  }
  return false;
}
void readInt(int &val){
  readWord();
  if(!(nextWord>>val)){
    if(!infile.eof()){
      throw ParseError(NUM_EXPECTED, linenum, charoffset) ;
    }
  }
  nextWord.clear();
  // val = temp;
  return ;
}

void readInstr(int &instr){
  readWord();

  if(!(nextWord>>instr)){

    // if(!infile.eof()){
    if(!infile.eof()||instr<1000){
      throw ParseError(ADDR_EXPECTED, linenum, charoffset-wordlen) ;
      exit(1);
    }
  }
  // instr = temp;
  return ;
}
void readChar(char &c){
  string temp;

  readWord(); //update the nextWord sstream, but now nextWord is empty

  if(!(nextWord>>temp)){ //can not read or read nothing

    if( ! infile.eof() ){
      throw ParseError(ADDR_EXPECTED, linenum, charoffset) ;
    }

  }
  // else{
  //   if(temp[0]!='A'&&temp[0]!='I'&&temp[0]!='R'&&temp[0]!='E'){
  //     throw ParseError(ADDR_EXPECTED, linenum, charoffset) ;
  //   }
  // }

  c=temp[0];

  return ;
}



void readDefList(){
  modulenum ++ ; //modulenum starts from 1
  moduleBaseAddr[modulenum]= baseAddr ;
  readInt(defcount);
  if(defcount>16){
    if(!infile.eof()){
    throw ParseError(TO_MANY_DEF_IN_MODULE,linenum, charoffset);
  }
  }
  totaldefcount += defcount; 
  string varname ;
  int raddr ;

  defList.clear();
  for (int i = 0; i < defcount; i++){
    readStr( varname );
    readInt( raddr );
    defList[varname] = raddr; //for testing too big 
    int abs = baseAddr + raddr;
    pair<string, int> p(varname, modulenum);  
    std::pair<std::map<string,int>::iterator,bool> ret;
    ret = symInModule.insert(p);
    if( ret.second == true ) {
      symAddr[varname] = abs;
      addrSym[abs] = varname ;
      ifUsed_deflist[varname] = false ; //init ifused deflist
      symInModule.insert(pair<string, int>(varname, modulenum));
    }else{
      multiDefSym.insert(varname);  //has already been defined 
    }
  }
}


void readUseList(){
  useList.clear() ;
  ifUsed_uselist.clear();
  readInt(usecount);
  // cout<<"usecount : "<<usecount<<endl;
  if(usecount>16){
    if(!infile.eof()){
      throw ParseError(TO_MANY_USE_IN_MODULE, linenum, charoffset);
    }
  }
  string sym;
  for(int i = 0; i<usecount; i++)
  {
    readStr(sym);
    useList.push_back(sym);
    alluseList.insert(sym) ;
    ifUsed_uselist[sym] = false; //initialized as unused
  }
}

void readInstrList(){
  readInt(codecount);
  baseAddr += codecount; //update the baseAddr
  string errormsg =""; 
  int opcode , relative, abs ;
  // char type; int instr;
  // cout<<codecount<<endl;

  for(int i= 0 ; i<codecount; i++){
    readChar(type);
    readInstr(instr);
    if(instr>9999) {
      instr = 9999;
      if(type == 'I'){
        errormsg = "Error: Illegal immediate value; treated as 9999" ;
        opcode = 9; abs = 999 ;
      }else{
        errormsg = "Error: Illegal opcode; treated as 9999";
        opcode = 9; abs = 999 ;
      }
    }
    else{
    opcode = instr/1000 ;
    relative = instr%1000 ;
    switch (type){
      case 'I': //no opcode or oprand
        abs = relative ;
        break ;

      case 'A': 
        abs = relative ;
        ifUsed_deflist[addrSym[abs]] = true;
        if( relative > MACHINE_SIZE ) {
          abs = 0;
          errormsg = "Error: Absolute address exceeds machine size; zero used";
        }
        break ;

      case 'R':
        if( relative > moduleSize[modulenum]) {
          relative = 0;
          errormsg = "Error: Relative address exceeds module size; zero used";
          abs = moduleBaseAddr[modulenum] + relative ;
          ifUsed_deflist[addrSym[abs]] = false;
        }else{
          abs = moduleBaseAddr[modulenum] + relative ;
          ifUsed_deflist[addrSym[abs]] = true;
        }

        break ;

      case 'E':
        if( relative >= useList.size() ) {
          abs = relative;
          errormsg = "Error: External address exceeds length of uselist; treated as immediate";
        }
        else {
          //find in deflist symAddr symbol useList[relative(index)]
          map<string,int>::iterator ret = symAddr.find(useList[relative]);
          if(ret == symAddr.end()) {
            errormsg = "Error: " + useList[relative] + " is not defined; zero used";
            abs = 0;
          }else {
            abs = ret->second; //the value is abs address of symbol
          }
          ifUsed_uselist[useList[relative]] = true;
          ifUsed_deflist[useList[relative]] = true;
        }
        break;
    } 
  }
    cout<<setfill('0') << setw(3)<<mem_num++<<": "; 
    cout << setfill('0') << setw(4) << (opcode*1000+abs);
    if( errormsg.length() > 0 ) {
      cout<<" "<<errormsg;
      errormsg = "";
    }
    cout<<endl;
  
  } //for loop end
  // print warning message for the unused variable

    
  const char* tmp ="Warning: Module %d: %s appeared in the uselist but was not actually used\n";

  for( map<string, bool>::iterator it = ifUsed_uselist.begin();it!= ifUsed_uselist.end(); it++){
    if(it->second == false && it->first.length()>0){
      printf(tmp, modulenum, it->first.c_str());
    }
  }
}


void getTotalInstr(){
  total_instr += codecount;
  
  if(total_instr> MACHINE_SIZE){
    if(!infile.eof()){
    throw  ParseError( TO_MANY_INSTR, linenum, charoffset );
    }
  }
  return ;
}

void printSymTable(){
  cout<< "Symbol Table"<<endl;

  // show content:
  for (std::map<string,int>::iterator it=symAddr.begin(); it!=symAddr.end(); ++it)
  {
    if(it->first.length()>0){
      cout << it->first << "=" << it->second;
      if( multiDefSym.count(it->first) > 0){
        cout << " "<<"Error: This variable is multiple times defined; first value used";
      }
    
      cout << endl;
    }
  }
  return;
}


void printmoduleBaseAddr(){
  // show content:
  for (std::map<int,int>::iterator it=moduleBaseAddr.begin(); it!=moduleBaseAddr.end(); ++it)
    std::cout << it->first << "=" << it->second << '\n';

  return;
}

