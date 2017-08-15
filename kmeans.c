//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
typedef float real;                    // Precision of float numbers
#define MAX_STRING 500
const long long max_size = 2000;         // max length of strings
const long long N = 150;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries
int classes = 0;
long long words, size;
float *M;

struct vocab_word {
  char *word;
};
struct vocab_word *vocab;
char kmeans_center_file[MAX_STRING];
char output_file[MAX_STRING];
int ArgPos(char *str, int argc, char **argv) {
  int a;
  for (a = 1; a < argc; a++) if (!strcmp(str, argv[a])) {
    if (a == argc - 1) {
      printf("Argument missing for %s\n", str);
      //exit(1);
    }
    return a;
  }
  return -1;
}
void PrintHelp(void){
    printf("\t-input vectors.bin file\n");
    printf("\t-format the vectors.bin file format\n");
    printf("\t\tUse <binary> to load binary file while others to load text file\n");
    printf("\t-output kmeans file\n");
    printf("\t-classes <int>\n");
	printf("\t-kmeans-center save file<path> \n");
}


void kmeans()
{
    // Run K-means on the word vectors
    int clcn = classes, iter = 10, closeid;
	int layer1_size = size;
	float * syn0 = M;
	FILE *fo;
	long long vocab_size = words+5;
    int *centcn = (int *)malloc(classes * sizeof(int));
    int *cl = (int *)calloc(vocab_size, sizeof(int));
    real closev, x, value;
	fo = fopen(output_file, "wb");
    real *cent = (real *)calloc(classes * layer1_size, sizeof(real));
	long a, b, c, d;
    printf("vocab_size %d\n",vocab_size); 
    for (a = 0; a < vocab_size; a++) cl[a] = a % clcn;
    for (a = 0; a < iter; a++) {
      for (b = 0; b < clcn * layer1_size; b++) cent[b] = 0;
      for (b = 0; b < clcn; b++) centcn[b] = 1;
      for (c = 0; c < vocab_size; c++) {
        for (d = 0; d < layer1_size; d++) cent[layer1_size * cl[c] + d] += syn0[c * layer1_size + d];
        centcn[cl[c]]++;
      }
      printf("test\n");
      for (b = 0; b < clcn; b++) {
        closev = 0;
        for (c = 0; c < layer1_size; c++) {
          cent[layer1_size * b + c] /= centcn[b];
          closev += cent[layer1_size * b + c] * cent[layer1_size * b + c];
        }
        closev = sqrt(closev);
        for (c = 0; c < layer1_size; c++) cent[layer1_size * b + c] /= closev;
      }

      printf("test2\n");
      for (c = 0; c < vocab_size; c++) {
        closev = -10;
        closeid = 0;
        for (d = 0; d < clcn; d++) {
          x = 0;
          for (b = 0; b < layer1_size; b++) x += cent[layer1_size * d + b] * syn0[c * layer1_size + b];
          if (x > closev) {
            closev = x;
            closeid = d;
          }
        }
        cl[c] = closeid;
      }
    }
    printf("kmeans computed start save\n");
    // Save the K-means classes
    for (a = 0; a < vocab_size; a++) fprintf(fo, "%s %d\n", vocab[a].word, cl[a]);
    printf("save center\n");
    fclose(fo);
  
    // Save the K-means classes center
    if(kmeans_center_file[0] != 0) {
        FILE *fo_center;
        real avg_dis = 0;
        int nodes = 0;
        fo_center = fopen(kmeans_center_file, "wb");
        for(a = 0; a < clcn ; a++){
            // class a
            avg_dis = 0;
            nodes = 0;
            fprintf(fo_center, "class %ld\n",a);
            // word in center and cos
            for(b = 0; b < vocab_size; b++){
                if( cl[b] == a){
                    x = 0;
                    value = 0;
                    nodes ++;
                    for(c = 0; c < layer1_size; c++) 
                    {
                        value += syn0[layer1_size * b +c] * syn0[layer1_size * b +c];
                        x += cent[ layer1_size * a + c] * syn0[layer1_size * b +c];
                    }
                    value = sqrt(value);
                    avg_dis += x/value;
                    fprintf(fo_center,"%s cos:%lf\n",vocab[b].word,x/value);
                    //for(c = 0; c < layer1_size; c++){
                    //    fprintf(fo_center," %lf ",syn0[layer1_size * b + c]);
                    //}
                }
            }
            avg_dis = avg_dis/nodes;
            fprintf(fo_center,"average distance %lf\n\n",avg_dis);
        }
        fclose(fo_center);
    }
  
    free(centcn);
    free(cent);
    free(cl);
  
}

int main(int argc, char **argv) {
  int format = 1;
  char format_str[50];
  FILE *f;
  char file_name[max_size];
  float  len ;
  long long  a, b ;
  int i;

  kmeans_center_file[0] = 0;
  output_file[0] = 0;
  if (argc < 3 || strcmp(argv[2],"help")==0 ||  strcmp(argv[2],"-help")==0 ||  strcmp(argv[2],"-h")==0 ) {
    printf("Usage: ./distance -format binary|text <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
	PrintHelp();
    return 0;
  }
  if ((i = ArgPos((char *)"-format", argc, argv)) > 0) {
	  strcpy(format_str, argv[i + 1]);
	  if(strcmp(format_str,"binary") == 0) format=0;
  }
  if ((i = ArgPos((char *)"-kmeans-center", argc, argv)) > 0) strcpy(kmeans_center_file, argv[i + 1]);
  if ((i = ArgPos((char *)"-input", argc, argv)) > 0) strcpy(file_name, argv[i + 1]);
  if ((i = ArgPos((char *)"-classes", argc, argv)) > 0) classes = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-output", argc, argv)) > 0) strcpy(output_file, argv[i + 1]);
  
  f = fopen(file_name, "rb");
  if (f == NULL) {
    printf("Input file not found\n");
    return -1;
  }
  printf("read vector\n");
  fscanf(f, "%lld", &words);
  fscanf(f, "%lld", &size);
  vocab = (struct vocab_word *)calloc(words+10, sizeof(struct vocab_word));
  for(a = 0; a <= words; a++) vocab[a].word = (char *)calloc(max_w, sizeof(char));
  M = (float *)malloc((long long)words * (long long)size * sizeof(float));
  if (M == NULL) {
    printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
    return -1;
  }
  if(format == 1){
		for (b = 0; b < words; b++) {
		a = 0;
		char ch;
		while (1) {
			ch = fgetc(f);
             if(a==0 && ch== ' ') continue;
             if(a==0 && ch== '\n') continue;
             vocab[b].word[a] = ch;
			vocab[b].word[a] = ch;
			if (feof(f) || (vocab[b].word[a] == ' ')) break;
			if ((a < max_w) && (vocab[b].word[a] != '\n')) a++;
		}
		vocab[b].word[a] = 0;
		for (a = 0; a < size; a++) {
			// modify by hanleyzhang 20160705
			//在进行word2vec训练时，-binary 1 时，词向量输出到vector.bin为二进制，否则为float. 
			//if(format == 1) 
			fscanf(f,"%f",&M[a + b * size]);//vector.bin 为float时，只能通过fscanf读取浮点向量数据
			//else
			//fread(&M[a + b * size], sizeof(float), 1, f);//vector.bin 为binary时，只能通过fread读取浮点向量数据。
			//fread(&M[a + b * size], sizeof(float), 1, f);
		}
		len = 0;
		for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
		len = sqrt(len);
		for (a = 0; a < size; a++) M[a + b * size] /= len;
	}  
  }
  else{
	for (b = 0; b < words; b++) {
		a = 0;
		while (1) {
		vocab[b].word[a] = fgetc(f);
		if (feof(f) || (vocab[b].word[a] == ' ')) break;
		if ((a < max_w) && (vocab[b].word[a] != '\n')) a++;
		}
		vocab[b].word[a] = 0;
		for (a = 0; a < size; a++) {
			// modify by hanleyzhang 20160705
			//在进行word2vec训练时，-binary 1 时，词向量输出到vector.bin为二进制，否则为float. 
			//if(format == 1) fscanf(f,"%f",&M[a + b * size]);//vector.bin 为float时，只能通过fscanf读取浮点向量数据
			//else
			fread(&M[a + b * size], sizeof(float), 1, f);//vector.bin 为binary时，只能通过fread读取浮点向量数据。
			//fread(&M[a + b * size], sizeof(float), 1, f);
		}
		len = 0;
		for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
		len = sqrt(len);
		for (a = 0; a < size; a++) M[a + b * size] /= len;
	}
  }
  fclose(f);
  printf("kmeans\n");
  kmeans(); 
  

  return 0;
}
