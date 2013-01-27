#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <Python.h>
#include <numpy/arrayobject.h>

#include "modules/vars.h"
#include "modules/skips.h"
#include "modules/galprop.h"
#include "modules/readpos.h"
#include "modules/readvel.h"
#include "modules/readpid.h"
#include "modules/readmass.h"
#include "modules/readu.h"
#include "modules/readrho.h"
#include "modules/readne.h"
#include "modules/readnh.h"
#include "modules/readhsml.h"
#include "modules/readsfr.h"
#include "modules/readage.h"
#include "modules/readmetal.h"
#include "modules/readfh2.h"
#include "modules/readsigma.h"
#include "modules/read_tipsy_only.h"


/*######################### READ HEADER ########################################*/
static PyObject *
readhead(PyObject *self, PyObject *args, PyObject *keywds)
{
  char* simtime   = "time";
  char* redshift  = "redshift";
  char* boxsize   = "boxsize";
  char* O0        = "O0";
  char* Ol        = "Ol";
  char* h         = "h";
  char* gascount  = "gascount";
  char* dmcount   = "dmcount";
  char* diskcount = "diskcount";
  char* bulgecount= "bulgecount";
  char* starcount = "starcount";
  char* bndrycount= "bndrycount";
  //FLAGS
  char* f_sfr     = "f_sfr";
  char* f_fb      = "f_fb";
  char* f_cooling = "f_cooling";
  char* f_age     = "f_age";
  char* f_metals  = "f_metals";
  char* Value;
  int value;

  j=0;
  NumFiles=1;
  Units=0;

  static char *kwlist[]={"file","value","numfiles","tipsy",NULL};
  if(!PyArg_ParseTupleAndKeywords(args,keywds,"ss|ii",kwlist,&filename,&Value,&NumFiles,&Tipsy)){
    PyErr_Format(PyExc_TypeError,"incorrect input!  must provide filename and value of interest - see readme.txt");
    //return NULL;
  }
  read_header();
  fclose(infp);  

  if(Tipsy==1){
    if(strcmp(Value,simtime)==0)        return Py_BuildValue("d",t_header.time);
    else if(strcmp(Value,redshift)==0)  return Py_BuildValue("d",(1./t_header.time)-1.);
    else if(strcmp(Value,gascount)==0)  return Py_BuildValue("i",t_header.ngas);
    else if(strcmp(Value,dmcount)==0)   return Py_BuildValue("i",t_header.ndark);
    else if(strcmp(Value,starcount)==0) return Py_BuildValue("i",t_header.nstar);
  }
  else{
    if(strcmp(Value,simtime)==0)        return Py_BuildValue("d",header.time);
    else if(strcmp(Value,redshift)==0)  return Py_BuildValue("d",header.redshift);
    else if(strcmp(Value,boxsize)==0)   return Py_BuildValue("d",header.BoxSize);
    else if(strcmp(Value,O0)==0)        return Py_BuildValue("d",header.Omega0);
    else if(strcmp(Value,Ol)==0)        return Py_BuildValue("d",header.OmegaLambda);
    else if(strcmp(Value,h)==0)         return Py_BuildValue("d",header.HubbleParam);
    else if(strcmp(Value,f_sfr)==0)     return Py_BuildValue("i",header.flag_sfr);
    else if(strcmp(Value,f_fb)==0)      return Py_BuildValue("i",header.flag_feedback);
    else if(strcmp(Value,f_cooling)==0) return Py_BuildValue("i",header.flag_cooling);
    else if(strcmp(Value,f_age)==0)     return Py_BuildValue("i",header.flag_stellarage);
    else if(strcmp(Value,f_metals)==0)  return Py_BuildValue("i",header.flag_metals);
    else if(strcmp(Value,gascount)==0)  return Py_BuildValue("i",header.npartTotal[0]);
    else if(strcmp(Value,dmcount)==0)   return Py_BuildValue("i",header.npartTotal[1]);
    else if(strcmp(Value,diskcount)==0) return Py_BuildValue("i",header.npartTotal[2]);
    else if(strcmp(Value,bulgecount)==0)return Py_BuildValue("i",header.npartTotal[3]);
    else if(strcmp(Value,starcount)==0) return Py_BuildValue("i",header.npartTotal[4]);
    else if(strcmp(Value,bndrycount)==0)return Py_BuildValue("i",header.npartTotal[5]);
  }
}

/*############################# DO SOME WORK! ###########################################*/
//PyArrayObject *array;
static PyObject *
readsnap(PyObject *self, PyObject *args, PyObject *keywds)
{
  j=0;
  NumFiles=1;
  Units=0;
  ERR=0;

  static char *kwlist[]={"file","data","type","numfiles","units","tipsy",NULL};
  if(!PyArg_ParseTupleAndKeywords(args,keywds,"sss|iii",kwlist,&filename,&Values,&Type,&NumFiles,&Units,&Tipsy)){
    PyErr_Format(PyExc_TypeError,"wrong input!  must provide filename, data block, and particle type of interest - see readme.txt");
    //return NULL;
  }

  if(Units>1 || Units<0){
    PyErr_Format(PyExc_IndexError,"Units flag must be 0 or 1!");
    //return NULL;
  }

  read_header();
  if(ERR==1){
    PyErr_Format(PyExc_TypeError,"readsnap: can't open file: '%s' ERR=%d",infile,ERR);
    //return NULL;
  }
  
  if(NumFiles!=header.num_files && Tipsy==0){
    PyErr_Format(PyExc_IndexError,"NumFiles(%d) != header.num_files(%d)!",NumFiles,header.num_files);
    //return NULL;
  }
  

  fclose(infp);
  assign_type();

  if(Tipsy==1) Units=1;

  printf("\ninput (%d files): %s \n",NumFiles,filename);
  printf("extracting %s data for %s\n",Values,Type);
  if(Units==0){
    if(values==5)  printf("returning PHYSICAL density in CODE units\n\n");
    if(values==13) printf("returning PHYSICAL surface density in CODE units\n\n");
    else           printf("returning code units\n\n");
  }
  if(Units==1){
    if(values==3)       printf("returning units of Msun \n\n");
    else if(values==4)  printf("returning units of Kelvin \n\n");
    else if(values==5)  printf("returning PHYSICAL density in units of g/cm^3 \n\n");
    else if(values==13) printf("returning PHYSICAL surface density in units of g/cm^2 \n\n");
    else                printf("returning code units\n\n");
  }
  
  //printf("j=%d\n",j);

  printf("values=%d\n",values);
  if(values==0)       readpos();
  else if(values==1)  readvel();
  else if(values==2)  readpid();
  else if(values==3)  readmass();
  else if(values==4)  readu();
  else if(values==5)  readrho();
  else if(values==6)  readNE();
  else if(values==7)  readNH();
  else if(values==8)  readHSML();
  else if(values==9)  readSFR();
  else if(values==10) readage();
  else if(values==11) readZ();
  else if(values==12) readfh2();
  else if(values==13) readsigma();
  else if(values==14) readmetals();
  else if(values==15 || values==16 || values==17 || values==18 || values==19) read_tipsy(values);
  else if(values==20 || values==21) read_tipsy_envira(values);
  else printf("houston we have a problem...no values returned\n");
  j=0;
  return PyArray_Return(array);
}


//Initialize Module
PyMethodDef methods[] = {
  {"test",test, METH_VARARGS ,"test function"},
  {"readsnap",readsnap,METH_VARARGS | METH_KEYWORDS, "readsnap info"},
  {"readhead",readhead,METH_VARARGS | METH_KEYWORDS, "read header data"},
  {"readfof",readfof,METH_VARARGS | METH_KEYWORDS, "read fof data"},
  {"galprop",galprop,METH_VARARGS | METH_KEYWORDS, "read galaxy property data"},
  {"galdata",galdata,METH_VARARGS | METH_KEYWORDS, "return galaxy particle info"},
  {"galdataonly",galdataonly,METH_VARARGS | METH_KEYWORDS, "return galaxy particle info (no indexes)"},
  {NULL,NULL,0,NULL}
};


PyMODINIT_FUNC
initreadgadget()
{
  (void) Py_InitModule("readgadget",methods);
  import_array();
}
