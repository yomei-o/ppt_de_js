/*
 * TinyJS
 *
 * A single-file Javascript-alike engine
 *
 * Authored By Gordon Williams <gw@pur3.co.uk>
 *
 * Copyright (C) 2009 Pur3 Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * This is a simple program showing how to use TinyJS
 */

#include "TinyJS.h"
#include "TinyJS_Functions.h"
#include "TinyJS_MathFunctions.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>


//const char *code = "var a = 5; if (a==5) a=4; else a=3;";
//const char *code = "{ var a = 4; var b = 1; while (a>0) { b = b * 2; a = a - 1; } var c = 5; }";
//const char *code = "{ var b = 1; for (var i=0;i<4;i=i+1) b = b * 2; }";
const char *code = "function myfunc(x, y) { return x + y; } var a = myfunc(1,2); print(a);";

void js_print(CScriptVar *v, void *userdata) {
    //printf("> %s\n", v->getParameter("text")->getString().c_str());
    printf("%s\n", v->getParameter("text")->getString().c_str());
}

void js_dump(CScriptVar *v, void *userdata) {
    CTinyJS *js = (CTinyJS*)userdata;
    js->root->trace(">  ");
}


int main(int argc, char **argv)
{
  int i;
  FILE* fp=NULL;
  char filename[1024];
  filename[0]=0;

  for(i=1;i<argc;i++){
    if(strcmp(argv[i],"-f")==0 && argv[i+1]!=NULL && 
      strlen(argv[i+1])+1<sizeof(filename))strcpy(filename,argv[i+1]);
  }
  fp=fopen(filename,"rt");
  if(filename[0] && fp==NULL){
	  fprintf(stderr,"Error: cannot open file\n");
	  return 1;
  }
  CTinyJS *js = new CTinyJS();
  /* add the functions from TinyJS_Functions.cpp */
  registerFunctions(js);

  /* add the math functions */
  registerMathFunctions(js);

  /* Add a native function */
  js->addNative("function print(text)", &js_print, 0);
  js->addNative("function dump()", &js_dump, js);
  /* Execute out bit of code - we could call 'evaluate' here if
     we wanted something returned */
  if(fp==NULL){
  try {
    js->execute("var lets_quit = 0; function quit() { lets_quit = 1; }");
    js->execute("print(\"Interactive mode... Type quit(); to exit, or print(...); to print something, or dump() to dump the symbol table!\");");
  } catch (CScriptException *e) {
    printf("ERROR: %s\n", e->text.c_str());
  }
  }
  if(fp==NULL){
  while (js->evaluate("lets_quit") == "0") {
    char buffer[2048];
    fgets ( buffer, sizeof(buffer), stdin );
    try {
      js->execute(buffer);
    } catch (CScriptException *e) {
      printf("ERROR: %s\n", e->text.c_str());
    }
  }
  }else{
	  char* buffer=NULL;
	  int sz,rsz;
	  fseek(fp,0,SEEK_END);
	  sz=ftell(fp);
	  fseek(fp,0,SEEK_SET);
	  buffer=(char*)malloc(sz+1);
	  buffer[0]=0;
	  buffer[sz]=0;
	  rsz=fread(buffer,1,sz,fp);
	  if(rsz>0)buffer[rsz]=0;
      try {
        js->execute(buffer);
      } catch (CScriptException *e) {
        printf("ERROR: %s\n", e->text.c_str());
      }
	  free(buffer);
  }


  delete js;
  if(fp!=NULL)fclose(fp);
  fp=NULL;
#ifdef _WIN32
#ifdef _DEBUG
  _CrtDumpMemoryLeaks();
#endif
#endif
  return 0;
}
