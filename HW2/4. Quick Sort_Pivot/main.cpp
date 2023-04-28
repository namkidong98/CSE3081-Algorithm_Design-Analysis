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
unsigned int swap_temp;
/******************************************************************************************************/

#define SWAP(a, b) {swap_temp = a; a = b; b = swap_temp;}

int Quick_Sort_P(unsigned int* data, int left, int right);
int Partition(unsigned int* data, int left, int right);
int MedianOfThree(unsigned int* data, int left, int right);
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
	int status = Quick_Sort_P(data, 0, data_num);

	if (status != 0) fprintf(stderr, "Sorting Error Occur\n"); //������ ���������� �̷������ ���� ���, �����޽����� ��� 	

	printf("Sorting Type: Quick_Sort_P\n"); //������ ����
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
	strcpy(file_write, "output_QS_P.bin"); //���ĵ� �����͸� ������ ���������� �̸�
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

int Quick_Sort_P(unsigned int* data, int left, int right) {
	//�迭�� ������ ũ�Ⱑ 3������ ��, ���� ����
	if (right - left <= 0) {
		return 0; //�迭�� ũ�Ⱑ 0�̸� �׳� ����
	}
	else if (right - left == 1) { //�迭�� ũ�Ⱑ 2�� �ٷ� ����
		if (data[left] > data[right]) {
			SWAP(data[left], data[right]);
		}
	}
	else if (right - left == 2) { //�迭�� ũ�Ⱑ 3�� ��, 3���� ���Ҹ� ���Ͽ� ����
		if (data[left] > data[left + 1]) {
			SWAP(data[left], data[left + 1]);
		}
		if (data[left] > data[right]) {
			SWAP(data[left], data[right]);
		}
		if (data[left + 1] > data[right]) {
			SWAP(data[left + 1], data[right]);
		}
	}

	//�迭�� ũ�Ⱑ 4 �̻��� ��, ������ ��� �Լ� ȣ��
	else {
		int Pivot = Partition(data, left, right);
		Quick_Sort_P(data, left, Pivot - 1);
		Quick_Sort_P(data, Pivot + 1, right);
	}

	if (left == 0 && right == data_num) return check(data, 0, data_num);
	else return 0;
}

int Partition(unsigned int* data, int left, int right) {
	int pivot = MedianOfThree(data, left, right);
	int i = left; int j = right - 1;

	while (true) {
		while (data[++i] < data[pivot]);
		while (j > left && data[--j] > data[pivot]);
		if (i >= j) break;
		SWAP(data[i], data[j]);
	}
	SWAP(data[i], data[pivot]);

	return i;
}

int MedianOfThree(unsigned int* data, int left, int right) {
	int center = (left + right) / 2;

	if (data[left] > data[center])
		SWAP(data[left], data[center]);
	if (data[left] > data[right])
		SWAP(data[left], data[right]);
	if (data[center] > data[right])
		SWAP(data[center], data[right]);
	SWAP(data[center], data[right - 1]);

	return right - 1;
}

int check(unsigned int* data, int left, int right) {
	CHECK_TIME_END(_start, _end, _freq, compute_time);
	for (int i = left; i < right - 1; i++) {
		if (data[i] > data[i + 1])  return -1; //�������� ���Ŀ� �����ϸ� -1����
	}
	return 0; //���������� ���ĵǸ� 1����
}