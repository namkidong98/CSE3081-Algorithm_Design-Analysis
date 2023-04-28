#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <math.h>

/******************************************************************************************************/
#include <Windows.h>
#define CHECK_TIME_START(start,freq) QueryPerformanceFrequency((LARGE_INTEGER*)&freq); QueryPerformanceCounter((LARGE_INTEGER*)&start)
#define CHECK_TIME_END(start,end,freq,time) QueryPerformanceCounter((LARGE_INTEGER*)&end); time = (float)((float)(end - start) / (freq * 1.0e-3f))

__int64 _start, _freq, _end;
float compute_time;
/******************************************************************************************************/
unsigned int swap_temp;
#define SWAP(a, b) {swap_temp = a; a = b; b = swap_temp;}

//#define N_ELEMENTS 1048576	// 2^20
//#define N_ELEMENTS 32768		// 2^15 
//#define N_ELEMENTS 1024		// 2^10
//#define N_ELEMENTS 32		// 2^5

//#define N_ELEMENTS 1024		// 2^10
//#define N_ELEMENTS 2048		// 2^11
//#define N_ELEMENTS 4096		// 2^12
//#define N_ELEMENTS 8192		// 2^13
//#define N_ELEMENTS 16384		// 2^14
//#define N_ELEMENTS 32768		// 2^15
//#define N_ELEMENTS 65536		// 2^16
//#define N_ELEMENTS 131072		// 2^17
//#define N_ELEMENTS 262144		// 2^18
#define N_ELEMENTS 524288		// 2^19

//#define FILE_NAME_PREFIX  "input_data_rd" // Random
//#define FILE_NAME_PREFIX  "input_data_as" // Ascending
//#define FILE_NAME_PREFIX  "input_data_de" // Descending
#define FILE_NAME_PREFIX  "input_data_fw" // Few swaps

void ascending(unsigned int* data, int left, int right);
void descending(unsigned int* data, int left, int right);
void few_swaps(unsigned int* data, int left, int right);

int n = N_ELEMENTS; // n is the number of data elements to be written

int main(int argc, char **arvgv) {
	unsigned scale_factor_uint = UINT_MAX >> 2;
	unsigned int* Arr;
	char file_name[128];
	unsigned int key;
	FILE *fp;
 
	Arr = (unsigned int*)malloc(sizeof(unsigned int) * n);
	sprintf(file_name, "%s_%d.bin", FILE_NAME_PREFIX, n);

	//fprintf(stdout, "^^^ UNIT_MAX = %u\n", UINT_MAX);
	//fprintf(stdout, "^^^ RAND_MAX = %u\n\n", RAND_MAX);

	srand((unsigned int)time(NULL));

	for (int i = 0; i < n; i++) {
		key = (unsigned int)((((float)rand()) / RAND_MAX) * (((float)rand()) / RAND_MAX) * (((float)rand()) / RAND_MAX) *
			(((float)rand()) / RAND_MAX) * (float)scale_factor_uint);
		Arr[i] = key; //랜덤하게 만든 숫자를 Arr 배열에 차례대로 저장
	}

	if ((fp = fopen(file_name, "wb")) == NULL) {
		fprintf(stderr, "Error: cannot open the binary file %s for writing...\n", file_name);
		exit(-1);
	}
	fwrite(&n, sizeof(unsigned int), 1, fp); //데이터의 개수를 우선 4바이트 크기로 저장

	if (!strcmp(FILE_NAME_PREFIX, "input_data_rd")) {
		for (int i = 0; i < n; i++) { //그대로 저장
			key = Arr[i];
			fwrite(&key, sizeof(unsigned int), 1, fp);
		}
	}
	if (!strcmp(FILE_NAME_PREFIX, "input_data_as")) {
		ascending(Arr, 0, n); //오름차순으로 정렬 먼저하고
		for (int i = 0; i < n; i++) { //저장
			key = Arr[i];
			fwrite(&key, sizeof(unsigned int), 1, fp);
		}
	}
	if (!strcmp(FILE_NAME_PREFIX, "input_data_de")) {
		descending(Arr, 0, n); //내림차순으로 정렬 먼저하고
		for (int i = 0; i < n; i++) { //저장
			key = Arr[i];
			fwrite(&key, sizeof(unsigned int), 1, fp);
		}
	}
	if (!strcmp(FILE_NAME_PREFIX, "input_data_fw")) {
		ascending(Arr, 0, n); //오름차순으로 정렬 먼저하고
		few_swaps(Arr, 0, n); //약간 섞어준 후
		for (int i = 0; i < n; i++) { //저장
			key = Arr[i];
			fwrite(&key, sizeof(unsigned int), 1, fp);
		}
	}

	fclose(fp);

	/*
	//저장된 파일이 제대로 되어 있는지 읽어내는 부분
	if ((fp = fopen(file_name, "rb")) == NULL) {
		fprintf(stderr, "Error: can not open the binary file %s for reading...\n", file_name);
		exit(-1);
	}
	fread(&n, sizeof(int), 1, fp); //저장되어 있는 데이터의 개수(n) 4바이트를 읽어낸다
	fprintf(stdout, "*** Number of elements = %d\n", n);

	for (int i = 0; i < n; i++) { //이후 n개의 데이터를 4바이트 단위씩 읽으면서 화면에 출력한다
		fread(&key, sizeof(unsigned int), 1, fp);
		fprintf(stdout, " [%d] %11u\n", i, key);
	}
	fclose(fp);
	*/

}

void ascending(unsigned int* data, int left, int right) {
	int i, j;
	unsigned tmp;
	for (i = left + 1; i < right; i++) {
		tmp = data[i];
		for (j = i; j > 0 && data[j - 1] > tmp; j--) {
			data[j] = data[j - 1];
		}
		data[j] = tmp;
	}
}

void descending(unsigned int* data, int left, int right) {
	int i, j;
	unsigned tmp;
	for (i = left + 1; i < right; i++) {
		tmp = data[i];
		for (j = i; j > 0 && data[j - 1] < tmp; j--) {
			data[j] = data[j - 1];
		}
		data[j] = tmp;
	}
}

void few_swaps(unsigned int* data, int left, int right) {
	int n = right - left; //배열의 크기
	int swap_count = (int)sqrt(n); //교환 횟수
	
	for (int k = 0; k < swap_count; k++) {
		// 랜덤 인덱스 i, j 선택
		int i = rand() % n + left;
		int j = rand() % n + left;

		// |i - j| < sqrt(n) 조건을 만족할 때까지 i, j 다시 선택
		while (abs(i - j) >= sqrt(n)) {
			i = rand() % n + left;
			j = rand() % n + left;
		}

		// i, j 번째 원소 교환
		unsigned int temp = data[i];
		data[i] = data[j];
		data[j] = temp;
	}
}