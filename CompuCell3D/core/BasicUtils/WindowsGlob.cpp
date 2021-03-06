#include "WindowsGlob.h"

static int glob_compare(const void *arg1, const void *arg2 )
{
  /* Compare all of both strings: */
  return stricmp( * (char ** ) arg1, * (  char ** ) arg2 );
}

 char * DLLCALL getfname( const   char * path)
{
 const   char * fname;
 const   char * bslash;

 fname=strrchr(path,'/');
 bslash=strrchr(path,'\\\\');
 if (bslash>fname)
 fname=bslash;
 if (fname!=NULL) 
 fname++;
 else  
 fname=( char *)path;
 return (( char *)fname);
}

 int DLLCALL glob( const   char  *pattern,  int  flags,  void * unused, glob_t* glob)
{
     struct _finddata_t ff;
 long ff_handle;
 size_t found=0;
 char path[MAX_PATH+1];
 char * p;
 char ** new_pathv;

 if (!(flags&GLOB_APPEND)) {
 glob->gl_pathc=0;
 glob->gl_pathv=NULL;
}

 ff_handle=_findfirst(( char *)pattern,&ff);
 while (ff_handle!=-1) {
 if (!(flags&GLOB_ONLYDIR) || ff.attrib&_A_SUBDIR) {
 if ((new_pathv=( char **)realloc(glob->gl_pathv
 ,(glob->gl_pathc+1)* sizeof ( char *)))==NULL) {
 globfree(glob);
 return (GLOB_NOSPACE);
}
 glob->gl_pathv=new_pathv;

 /* build the full pathname */
 //SAFECOPY(path,pattern);
 strcpy(path, pattern);
 p=getfname(path);
 *p=0;
 strcat(path,ff.name);

 if ((glob->gl_pathv[glob->gl_pathc]=( char *)malloc(strlen(path)+2))==NULL) {
 globfree(glob);
 return (GLOB_NOSPACE);
}
 strcpy(glob->gl_pathv[glob->gl_pathc],path);
 if (flags&GLOB_MARK && ff.attrib&_A_SUBDIR)
 strcat(glob->gl_pathv[glob->gl_pathc],"/");

 glob->gl_pathc++;
 found++;
}
 if (_findnext(ff_handle, &ff)!=0) {
 _findclose(ff_handle);
 ff_handle=-1; 
} 
}

 if (found==0)
 return (GLOB_NOMATCH);

 if (!(flags&GLOB_NOSORT)) {
 qsort(glob->gl_pathv,found, sizeof ( char *),glob_compare);
}

 return (0); /* success */
}
 void  DLLCALL globfree(glob_t* glob)
{
 size_t i;

 if (glob==NULL)
 return ;

 if (glob->gl_pathv!=NULL) {
 for (i=0;i<glob->gl_pathc;i++)
 if (glob->gl_pathv[i]!=NULL)
 free(glob->gl_pathv[i]);

 free(glob->gl_pathv);
 glob->gl_pathv=NULL;
}
glob->gl_pathc=0;
}
 