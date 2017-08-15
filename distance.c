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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

const long long max_size = 2000;         // max length of strings
const long long N = 150;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries
int relative_distance = 0;

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
    printf("\t-relative-distance locate vector distance compute\n");
}

int main(int argc, char **argv) {
  printf("hello\n");
  int format = 1;
  char format_str[50];
  FILE *f;
  char st1[max_size];
  char *bestw[N];
  char file_name[max_size], st[100][max_size];
  float dist, len, bestd[N], vec[max_size];
  long long words, size, a, b, c, d, cn, bi[100];
  int i;
  float *M;
  char *vocab;
  
  if (argc < 3 || strcmp(argv[2],"help")==0 ||  strcmp(argv[2],"-help")==0 ||  strcmp(argv[2],"-h")==0 ) {
    printf("Usage: ./distance -format binary|text <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
	PrintHelp();
    return 0;
  }
  if ((i = ArgPos((char *)"-format", argc, argv)) > 0) {
	  strcpy(format_str, argv[i + 1]);
	  if(strcmp(format_str,"binary") == 0) format=0;
  }
  if ((i = ArgPos((char *)"-input", argc, argv)) > 0) strcpy(file_name, argv[i + 1]);
  if ((i = ArgPos((char *)"-relative-distance", argc, argv)) > 0) relative_distance = atoi(argv[i + 1]);
  
  f = fopen(file_name, "rb");
  if (f == NULL) {
    printf("Input file not found\n");
    return -1;
  }
  fscanf(f, "%lld", &words);
  fscanf(f, "%lld", &size);
  vocab = (char *)malloc((long long)words * max_w * sizeof(char));
  for (a = 0; a < N; a++) bestw[a] = (char *)malloc(max_size * sizeof(char));
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
             vocab[b * max_w + a] = ch;
			vocab[b * max_w + a] = ch;
			if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
			if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
		}
		vocab[b * max_w + a] = 0;
		for (a = 0; a < size; a++) {
			// modify by hanleyzhang 20160705
			//在进行word2vec训练时，-binary 1 时，词向量输出到vector.bin为二进制，否则为float. 
			//if(format == 1) 
			fscanf(f,"%f",&M[a + b * size]);//vector.bin 为float时，只能通过fscanf读取浮点向量数据
			//else
			//fread(&M[a + b * size], sizeof(float), 1, f);//vector.bin 为binary时，只能通过fread读取浮点向量数据。
			//fread(&M[a + b * size], sizeof(float), 1, f);
		}
		//if(relative_distance == 0){
			len = 0;
			for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
			len = sqrt(len);
			for (a = 0; a < size; a++) M[a + b * size] /= len;
		//}
	}  
  }
  else{
	for (b = 0; b < words; b++) {
		a = 0;
		while (1) {
		vocab[b * max_w + a] = fgetc(f);
		if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
		if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
		}
		vocab[b * max_w + a] = 0;
		for (a = 0; a < size; a++) {
			// modify by hanleyzhang 20160705
			//在进行word2vec训练时，-binary 1 时，词向量输出到vector.bin为二进制，否则为float. 
			//if(format == 1) fscanf(f,"%f",&M[a + b * size]);//vector.bin 为float时，只能通过fscanf读取浮点向量数据
			//else
			fread(&M[a + b * size], sizeof(float), 1, f);//vector.bin 为binary时，只能通过fread读取浮点向量数据。
			//fread(&M[a + b * size], sizeof(float), 1, f);
		}
		//if(relative_distance == 0){
			len = 0;
			for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
			len = sqrt(len);
			for (a = 0; a < size; a++) M[a + b * size] /= len;
		//}
	}
  }
  fclose(f);
  while (1) {
    for (a = 0; a < N; a++) bestd[a] = 0;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    printf("Enter words or sentence (EXIT to break): ");
    a = 0;
    while (1) {
      st1[a] = fgetc(stdin);
      if ((st1[a] == '\n') || (a >= max_size - 1)) {
        st1[a] = 0;
        break;
      }
      a++;
    }
    if (!strcmp(st1, "EXIT")) break;
    cn = 0;
    b = 0;
    c = 0;
    while (1) {
      st[cn][b] = st1[c];
      b++;
      c++;
      st[cn][b] = 0;
      if (st1[c] == 0) break;
      if (st1[c] == ' ') {
        cn++;
        b = 0;
        c++;
      }
    }
    cn++;
    for (a = 0; a < cn; a++) {
      for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
      if (b == words) b = -1;
      bi[a] = b;
      //printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
      if (b == -1) {
        printf("Out of dictionary word!\n");
        break;
      }
    }
    if (b == -1) continue;
	if(relative_distance == 0){
        printf("\n                                              Word %lld       Cosine distance\n------------------------------------------------------------------------\n",N);
        for (a = 0; a < size; a++) vec[a] = 0;
        for (b = 0; b < cn; b++) {
          if (bi[b] == -1) continue;
          for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
        }
        len = 0;
        for (a = 0; a < size; a++) len += vec[a] * vec[a];
        len = sqrt(len);
        for (a = 0; a < size; a++) vec[a] /= len;
        for (a = 0; a < N; a++) bestd[a] = -1;
        for (a = 0; a < N; a++) bestw[a][0] = 0;
        for (c = 0; c < words; c++) {
          a = 0;
          for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
          if (a == 1) continue;
          dist = 0;
          for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];
          for (a = 0; a < N; a++) {
            if (dist > bestd[a]) {
              for (d = N - 1; d > a; d--) {
                bestd[d] = bestd[d - 1];
                strcpy(bestw[d], bestw[d - 1]);
              }
              bestd[a] = dist;
              strcpy(bestw[a], &vocab[c * max_w]);
              break;
            }
          }
        }
        for (a = 0; a < N; a++) printf("%50s\t\t%f\n", bestw[a], bestd[a]);
    }
    else {
        if(cn < 4) {printf("need input 4 words\n");}
        float * wordvector[6];
		float len1 = 0,len2 = 0;
		float relative_distance = 0;
		for(a = 0; a < 6 ; a++) { wordvector[a] = (float *)malloc( (long long)size * sizeof(float));}
        int find = 0;
        for(a = 0; a < words ; a++ ){
            if (!strcmp(&vocab[a * max_w], st[0])) { for(b = 0; b < size;b++){ wordvector[0][b] = M[a * max_w + b]; } find++;}
            if (!strcmp(&vocab[a * max_w], st[1])) { for(b = 0; b < size;b++){ wordvector[1][b] = M[a * max_w + b]; } find++;}
            if (!strcmp(&vocab[a * max_w], st[2])) { for(b = 0; b < size;b++){ wordvector[2][b] = M[a * max_w + b]; } find++;}
            if (!strcmp(&vocab[a * max_w], st[3])) { for(b = 0; b < size;b++){ wordvector[3][b] = M[a * max_w + b]; } find++;}
            if(find >=4) { break;}
        }
		for(b = 0; b < size ; b++) 
		{
			wordvector[4][b] = wordvector[0][b] - wordvector[1][b];
			wordvector[5][b] = wordvector[2][b] - wordvector[3][b];
		}
		for(b = 0; b < size ; b++) {
			len1 += wordvector[4][b]*wordvector[4][b];
			len2 += wordvector[5][b]*wordvector[5][b];
		}
		len1 = sqrt(len1); len2 = sqrt(len2);
		for(b = 0 ; b < size ; b++){
			wordvector[4][b] /= len1;
			wordvector[5][b] /= len2;
		}
		for( b = 0;b < size ; b++){relative_distance += wordvector[4][b]*wordvector[5][b];}
		printf("cos( (%s - %s), (%s - %s) ) is %lf\n",st[0],st[1],st[2],st[3],relative_distance);
    }
	
  }
  return 0;
}
