// appendtail.exe.cpp : 定義主控台應用程式的進入點。
//
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <io.h>

static char appendix[1024]="# This is my signature.";
static char buffer[1024];

int main(int argc, TCHAR* argv[])
{
	FILE	*out=NULL;
	TCHAR	*src=NULL,*dst=NULL, *myname=argv[0];
	bool	remove=false, isappended=false;
	size_t	len;
	int		rt=-1;

	for(argc--,argv++;argc>0;argc--,argv++) {
		switch(**argv) {
			case '+':
				remove=false;
				if((*argv)[1])
					src=(*argv)+1;
				break;

			case '-':
				remove=true;
				if((*argv)[1])
					src=(*argv)+1;
				break;

			default:
				dst=*argv;
				break;
		}
	}

	if(dst==NULL) {
		fwprintf(stderr,L"Usage: %s [+/-][sigfile] <target file>\n",myname);
		goto quit;
	}

	// read source file
	if(src) {
		FILE *fp=_wfopen(src,L"rb");
		if(fp==NULL) {
			fwprintf(stderr,L"Error: cannot open file %s\n",src);
			goto quit;
		}

		size_t n=fread(appendix,1,1023,fp);
		fclose(fp);
		if(n<=0) {
			fwprintf(stderr,L"Error: can't read file %s\n",src);
			goto quit;
		}

		appendix[n]=0;
	}
	len=strlen(appendix);

	out=_wfopen(dst,L"r+b");
	if(out==NULL) {
		fwprintf(stderr,L"Cannot open output file: %s\n",dst);
		goto quit;
	}

	if(fseek(out,-(int)len,SEEK_END)!=0) {
		fwprintf(stderr,L"Read signature error.\n");
		goto quit;
	}

	if(fread(buffer,1,len,out)<len) {
		fwprintf(stderr,L"Read signature error.\n");
		goto quit;
	}
	buffer[len]=0;
	isappended=(strcmp(buffer,appendix)==0);

	if(remove) {
		if(isappended) {
			int size=ftell(out)-len;
			rt=_chsize_s(_fileno(out),size);
		} else {
			rt=0;
		}
	} else {
		if(isappended) {
			rt=0;
		} else {
			if(fseek(out,0,SEEK_END)!=0 || fwrite(appendix,len,1,out)<1)
				fwprintf(stderr,L"Error writing file.\n");
			else
				rt=0;
		}
	}

quit:
	if(out) fclose(out);
	return rt;
}
