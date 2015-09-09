#include <stdio.h> 

int main()
{
	FILE *fp;
	
	//rt:t表示以文本方式打开，b表示以二进制方式打开，默认为t
	if((fp=fopen("timing.h","rt+"))==NULL)
	{
		printf("\nCannot open file strike any key exit!");
		getchar();
		exit(1);
	}

	/* method 1: ReadByChar
	char ch;
	ch=fgetc(fp);
	while(ch!=EOF)
	{
		putchar(ch);
		ch=fgetc(fp);
	}
	fclose(fp);
	*/

	/* method 2: ReadByLine
	//gets()  and  fgets() return s on success, and NULL on error or when end
       	//	of file occurs while no characters have been read.
	//char *fgets(char *s, int size, FILE *stream);*/
	char *s;
	s = fgets(s, 100, fp);
	while(s!=NULL)
	{
		puts(s);
		s = fgets(s, 100, fp);
	}
	fclose(fp);
	
	return 0;
}
