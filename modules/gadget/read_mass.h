#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <Python.h>
#include <numpy/arrayobject.h>

/*######################### MASSES ########################################*/
void gadget_mass()
{  
  float *simdata;
  int ndim = 1;

  int i;
  unsigned int n;
  int k;
  unsigned int pc = 0;

  unsigned int skip1, skip2;
  char* blocklabel = "MASS";

  double convert = UnitMass_in_g / SOLARMASS;

  for(j=0;j<NumFiles;j++){
    skip_blocks(values);
    if(j==0){
      npy_intp dims[1]={header.npartTotal[type]};
      array = (PyArrayObject *)PyArray_SimpleNew(ndim,dims,PyArray_DOUBLE);
    }
    /*
      if(Units==1){
      printf("### RETURNING MASS IN GADGET UNITS, MULTIPLY BY 1.98892e43 TO CONVER TO GRAMS ###\n");
      }
    */
    
    if(header.mass[type]>0 && header.npart[type]>0){
      printf("non-zero header mass detected - using header mass for %s\n",Type);
      for(n=0;n<header.npart[type];n++)
	{
	  if(Units==0) MDATA(array,pc)=header.mass[type];
	  if(Units==1) MDATA(array,pc)=header.mass[type] * convert;
	  pc++;
	}
    }
    else{
      printf("reading mass block for %s\n",Type);
      simdata=(float*)malloc(header.npart[type]*sizeof(float));
      
      fread(&skip1,sizeof(int),1,infp);
      //seek past particle groups not interested in
      if(type==1){
	if(header.mass[0]==0)
	  fseek(infp, header.npart[0]*sizeof(float),SEEK_CUR);
      }
      if(type==2){
	for(k=0;k<2;k++){
	  if(header.mass[k]==0)
	    fseek(infp,header.npart[k]*sizeof(float),SEEK_CUR);
	}
      }      
      if(type==3){
	for(k=0;k<3;k++){
	  if(header.mass[k]==0)
	    fseek(infp,header.npart[k]*sizeof(float),SEEK_CUR);
	}
      } 
      if(type==4){
	for(k=0;k<4;k++){
	  if(header.mass[k]==0)
	    fseek(infp,header.npart[k]*sizeof(float),SEEK_CUR);
	}
      }
      if(type==5){
	for(k=0;k<5;k++){
	  if(header.mass[k]==0)
	    fseek(infp,header.npart[k]*sizeof(float),SEEK_CUR);
	}
      }
      //if(type==4) fseek(infp,header.npart[0]*sizeof(float),SEEK_CUR);
      fread(simdata,header.npart[type]*sizeof(float),1,infp);
      fread(&skip2,sizeof(int),1,infp);
      errorcheck(skip1,skip2,blocklabel);
      fclose(infp);
      
      //count = count + header.npart[type];
      for(n=0;n<header.npart[type];n++)
	{
	  if(Units==0) MDATA(array,pc) = simdata[n];
	  if(Units==1) MDATA(array,pc) = simdata[n] * convert;
	  pc++;
	}
    }
  }
  if(pc!=header.npartTotal[type]){
    PyErr_Format(PyExc_IndexError,"particle count mismatch! pc=%d  npartTotal[%d]=%d",pc,type,header.npartTotal[type]);
    //return NULL;
  }
  return;
}
