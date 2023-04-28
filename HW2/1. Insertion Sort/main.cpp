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

int Insertion_Sort(unsigned int* data, int left, int right);
int check(unsigned int* data, int left, int right);

int data_num; //���Ͽ� ����Ǿ� �ִ� �������� ����

int main() {
	FILE* fp = fopen(FILE_NAME, "rb"); //rb - ���� �������� �б� ���
	unsigned int* data;
	unsigned int temp; //���Ͽ��� ���� ���� �� ����ϴ� ����

	//���� �տ� ����� 4����Ʈ(unsigned int)�� ����� ������ ����(N)�� �д´�
	fread(&data_num, sizeof(unsigned int), 1, fp);
	data = (unsigned int*)malloc(sizeof(unsigned int) * data_num); //���� ũ�⸸ŭ ���� ������ �� �ִ� data �迭 �Ҵ�

	for (int i = 0; i < data_num; i++) {
		fread(&temp, sizeof(int), 1, fp);
		data[i] = temp;
	}

	CHECK_TIME_START(_start, _freq);
	int status = Insertion_Sort(data, 0, data_num);


	if (status != 0) fprintf(stderr, "Sorting Error Occur\n"); //������ ���������� �̷������ ���� ���, �����޽����� ��� 	

	printf("Sorting Type: Insertion_Sort\n"); //������ ����
	printf("Number of Data : % d\n", data_num); //���Ŀ� ���Ǵ� �������� ����
	printf("Time : % .3f ms\n\n", compute_time); //���Ŀ� �ҿ�� �ð�(ms����)

	/*
	for (int i = 0; i < data_num; i++) { //���ĵ� �����͸� ó������ ���κ��� ���
		printf("%d data = %d\n", i, data[i]);
	}
	*/

	fclose(fp);

	//-----------������ �迭�� ���� ���Ͽ� �����ϴ� �۾�---------------//
	char file_write[20]; unsigned int key;
	strcpy(file_write, "output_IS.bin"); //���ĵ� �����͸� ������ ���������� �̸�
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

int Insertion_Sort(unsigned int* data, int left, int right) {
	int i, j;
	unsigned temp;

	for (i = left + 1; i < right; i++) {
		temp = data[i];
		for (j = i; j > 0 && data[j - 1] > temp; j--) {
			data[j] = data[j - 1];
		}
		data[j] = temp;
	}

	return check(data, 0, data_num);
}

int check(unsigned int* data, int left, int right) {
	CHECK_TIME_END(_start, _end, _freq, compute_time);
	for (int i = left; i < right - 1; i++) {
		if (data[i] > data[i + 1])  return -1; //�������� ���Ŀ� �����ϸ� -1����
	}
	return 0; //���������� ���ĵǸ� 1����
}