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
	int status = Merge_Sort(data, 0, data_num);
	if (status != 0) fprintf(stderr, "Sorting Error Occur\n"); //������ ���������� �̷������ ���� ���, �����޽����� ��� 	

	printf("Sorting Type: Merge_Sort\n"); //������ ����
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
	strcpy(file_write, "output_MS.bin"); //���ĵ� �����͸� ������ ���������� �̸�
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
		if (data[i] > data[i + 1])  return -1; //�������� ���Ŀ� �����ϸ� -1����
	}
	return 0; //���������� ���ĵǸ� 1����
}