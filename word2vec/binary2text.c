//sa614349@mail.ustc.edu.com
//desc:word2vec output vectors.bin file inter-transformation of binary and text file


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

const long long max_w = 50;              // max length of vocabulary entries
int format = 1;//1: binary to text other:text to binary 
const int file_name_len=200;
char read_file[200];
char write_file[200];

int ArgPos(char *str, int argc, char **argv) {
  int a;
  for (a = 1; a < argc; a++) if (!strcmp(str, argv[a])) {
    if (a == argc - 1) {
      printf("Argument missing for %s\n", str);
      exit(1);
    }
    return a;
  }
  return -1;
}

void PrintHelp(void){
    printf("\t-input vectors.bin file\n");
    printf("\t-output the transformated file\n");
    printf("\t-format binary2text|text2binary  to inter-conversion from binary to text file  \n");
}


int GetInput(int argc, char **argv)
{
  int i;
  char format_char[50];
    
  if ((i = ArgPos((char *)"-format", argc, argv)) > 0) {strcpy(format_char,argv[i + 1]); if(strcmp(format_char,"text2binary") == 0) format=0 ;}
  if ((i = ArgPos((char *)"-input", argc, argv)) > 0) strcpy(read_file, argv[i + 1]);
  if ((i = ArgPos((char *)"-output", argc, argv)) > 0) strcpy(write_file, argv[i + 1]);
}

int main(int argc, char **argv) {
    if (argc < 4 || strcmp(argv[1],"help")==0 ||  strcmp(argv[1],"-help")==0 ||  strcmp(argv[1],"-h")==0 ) {
        printf("Usage: ./binary2text -input <file> -output <file> format <text2binary|binary2text>\n");
	    PrintHelp();
        return -1;
    }
	GetInput(argc,argv);
	FILE *f,*fo;
	long long words, size, a, b,  cn;
	char ch;
	float *M;
	char *vocab;
	f = fopen(read_file, "rb");
	fo = fopen(write_file, "wb");
	if (f == NULL) {
		printf("Input file not found\n");
		return -1;
	}
    if (fo == NULL) {
        printf("Output file error\n");
        return -1;
    }
	fscanf(f, "%lld", &words);
	fscanf(f, "%lld", &size);
	vocab = (char *)malloc((long long)max_w * sizeof(char));
	M = (float *)malloc((long long)words * (long long)size * sizeof(float));
	if (M == NULL || vocab == NULL ) {
		printf("Cannot allocate memory");
		return -1;
	}
    
	fprintf(fo, "%lld %lld\n", words, size);
	//binary to text format
	if (format == 1) {
		long long  times = words;
        char ch;
		while(times--)
		{
			if(feof(f)) break;
			memset(vocab,0,sizeof(vocab));
			a = 0;
			while (1) {
				ch = fgetc(f);
                if(ch=='\n' && a==0) continue;
                vocab[a] = ch;
				if (feof(f) || (vocab[a] == ' ')) break;
				if ((a < max_w) && (vocab[max_w + a] != '\n')) a++;
			}
			vocab[a] = 0;
			for (a = 0; a < size; a++) {
				// modify by hanleyzhang 20160705
				//在进行word2vec训练时，-binary 1 时，词向量输出到vector.bin为二进制，否则为float. 
				//fscanf(f,"%f",&M[a + b * size]);//vector.bin 为float时，只能通过fscanf读取浮点向量数据
				fread(&M[a], sizeof(float), 1, f);//vector.bin 为binary时，只能通过fread读取浮点向量数据。
				//fread(&M[a + b * size], sizeof(float), 1, f);
			}
			fprintf(fo, "%s ", vocab);
			for (b = 0; b < size; b++) fprintf(fo, "%lf ", M[b]);
			fprintf(fo,"\n");
		}
	}
	//text to binary
	else{
		long long  times = words;
        char ch;
		while(times--)
		{
			if(feof(f)) break;
			memset(vocab,0,sizeof(vocab));
			a = 0;
			while (1) {

				ch = fgetc(f);
                if(a==0 && ch== ' ') continue;
                if(a==0 && ch== '\n') continue;
                vocab[a] = ch;
				if (feof(f) || (vocab[a] == ' ')) break;
				if ((a < max_w) && (vocab[max_w + a] != '\n')) a++;
			}
			vocab[a] = 0;
            //if(a < 1) continue;
			for (b = 0; b < size; b++) {
				// modify by hanleyzhang 20160705
				//在进行word2vec训练时，-binary 1 时，词向量输出到vector.bin为二进制，否则为float. 
				fscanf(f,"%f",&M[b]);//vector.bin 为float时，只能通过fscanf读取浮点向量数据
				//fread(&M[a], sizeof(float), 1, f);//vector.bin 为binary时，只能通过fread读取浮点向量数据。
			}
            //if(a==0) printf("%s\n",vocab)
			fprintf(fo, "%s ", vocab);
			for (b = 0; b < size; b++) fwrite(&M[b], sizeof(float), 1, fo);
			fprintf(fo,"\n");
		}
	}
  fclose(f);
  fclose(fo);
  return 0;
}
