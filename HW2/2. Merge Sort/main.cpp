#define FILE_NAME "input_data_rd_1024.bin"	//2^10

#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>

/******************************************************************************************************/
#include <Windows.h>
#define CHECK_TIME_START(start,freq) QueryPerformanceFrequency((LARGE_INTEGER*)&freq); QueryPerformanceCounter((LARGE_INTEGER*)&start)
#define CHECK_TIME_END(start,end,freq,time) QueryPerformanceCounter((LARGE_INTEGER*)&end); time = (float)((float)(end - start) / (freq * 1.0e-3f))

__int64 _start, _freq, _end;
float compute_time;
int swap_temp;
/******************************************************************************************************/

#define SWAP(a, b) {\swap_temp = a; a = b; b = swap_temp;\}

int Merge_Sort(unsigned int* data, int left, int right);
void Merge(unsigned int* data, int left, int middle, int right);
int check(unsigned int* data, int left, int right);

int data_num; //파일에 저장되어 있는 데이터의 개수

int main() {
	FILE* fp = fopen(FILE_NAME, "rb"); //rb - 이진 형식으로 읽기 모드
	unsigned int* data;
	unsigned int temp; //파일에서 값을 받을 때 사용하는 변수

	//파일 앞에 저장된 4바이트(unsigned int)의 저장된 데이터 개수(N)를 읽는다
	fread(&data_num, sizeof(unsigned int), 1, fp);
	data = (unsigned int*)malloc(sizeof(unsigned int) * data_num); //읽은 크기만큼 값을 저장할 수 있는 data 배열 할당

	for (int i = 0; i < data_num; i++) {
		fread(&temp, sizeof(int), 1, fp);
		data[i] = temp;
	}

	CHECK_TIME_START(_start, _freq);
	int status = Merge_Sort(data, 0, data_num);
	if (status != 0) fprintf(stderr, "Sorting Error Occur\n"); //정렬이 성공적으로 이루어지지 않은 경우, 에러메시지를 출력 	

	printf("Sorting Type: Merge_Sort\n"); //정렬의 종류
	printf("Number of Data : % d\n", data_num); //정렬에 사용되는 데이터의 개수
	printf("Time : % .3f ms\n\n", compute_time); //정렬에 소요된 시간(ms단위)


	/*
	for (int i = 0; i < data_num; i++) { //정렬된 데이터를 처음부터 라인별로 출력
		printf("%d data = %d\n", i, data[i]);
	}
	*/

	fclose(fp);


	//-----------정렬한 배열을 이진 파일에 저장하는 작업---------------//
	char file_write[20]; unsigned int key;
	strcpy(file_write, "output_MS.bin"); //정렬된 데이터를 저장할 이진파일의 이름
	if ((fp = fopen(file_write, "wb")) == NULL) {
		fprintf(stderr, "Error: cannot open the binary file %s for writing...\n", file_write);
		exit(-1);
	}
	fwrite(&data_num, sizeof(unsigned int), 1, fp);
	for (int i = 0; i < data_num; i++) {
		key = data[i];
		fwrite(&key, sizeof(unsigned int), 1, fp);
	}
	fclose(fp);
	//-----------------------------------------------------------------//

	free(data);
	return 0;
}

int Merge_Sort(unsigned int* data, int left, int right) {
	int middle;
	if (left < right) {
		middle = (left + right) / 2;

		Merge_Sort(data, left, middle);
		Merge_Sort(data, middle + 1, right);

		Merge(data, left, middle, right);
	}

	if (left == 0 && right == data_num) return check(data, 0, data_num);
	else return 0;
}

void Merge(unsigned int* data, int left, int middle, int right) {
	int n1 = middle - left + 1;
	int n2 = right - middle;

	unsigned int* L = (unsigned int*)malloc(sizeof(unsigned int) * n1);
	unsigned int* R = (unsigned int*)malloc(sizeof(unsigned int) * n2);

	for (int i = 0; i < n1; i++)
		L[i] = data[left + i];
	for (int j = 0; j < n2; j++)
		R[j] = data[middle + 1 + j];

	int i = 0, j = 0;
	int k = left;
	while (i < n1 && j < n2) {
		if (L[i] <= R[j])
			data[k++] = L[i++];
		else
			data[k++] = R[j++];
	}

	while (i < n1)
		data[k++] = L[i++];
	while (j < n2)
		data[k++] = R[j++];

	free(L);
	free(R);
}

int check(unsigned int* data, int left, int right) {
	CHECK_TIME_END(_start, _end, _freq, compute_time);
	for (int i = left; i < right - 1; i++) {
		if (data[i] > data[i + 1])  return -1; //오름차순 정렬에 위배하면 -1리턴
	}
	return 0; //정상적으로 정렬되면 1리턴
}