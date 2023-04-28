#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>

/******************************************************************************************************/
#include <Windows.h>
#define CHECK_TIME_START(start,freq) QueryPerformanceFrequency((LARGE_INTEGER*)&freq); QueryPerformanceCounter((LARGE_INTEGER*)&start)
#define CHECK_TIME_END(start,end,freq,time) QueryPerformanceCounter((LARGE_INTEGER*)&end); time = (float)((float)(end - start) / (freq * 1.0e-3f))

__int64 _start, _freq, _end;
float compute_time;
/******************************************************************************************************/

#define N_ELEMENTS 1048576	// 2^20
//#define N_ELEMENTS 1024		// 2^10
//#define N_ELEMENTS 32		// 2^5


//#define FILE_NAME_PREFIX  "input_data_rd" // Random
//#define FILE_NAME_PREFIX  "input_data_as" // Ascending
//#define FILE_NAME_PREFIX  "input_data_de" // Descending
//#define FILE_NAME_PREFIX  "input_data_fw" // Few swaps

int main(int argc, char** arvgv) {
	char file_name[128];
	FILE* fp;

	int n = N_ELEMENTS; // n is the number of data elements to be written
	sprintf(file_name, "%s_%d.bin", FILE_NAME_PREFIX, n);

	if ((fp = fopen(file_name, "rb")) == NULL) {
		fprintf(stderr, "Error: can not open the binary file %s for reading...\n", file_name);
		exit(-1);
	}
	fread(&n, sizeof(int), 1, fp); //저장되어 있는 데이터의 개수(n) 4바이트를 읽어낸다
	fprintf(stdout, "*** Number of elements = %d\n", n);

	for (int i = 0; i < n; i++) { //이후 n개의 데이터를 4바이트 단위씩 읽으면서 화면에 출력한다
		unsigned int key;
		fread(&key, sizeof(unsigned int), 1, fp);
		fprintf(stdout, " [%d] %11u\n", i, key);
	}
	fclose(fp);
}