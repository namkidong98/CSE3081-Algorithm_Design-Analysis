#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define FILENAME_MAX 260
#define MAX_ELEMENT 201

void getinfo_element(char* input);
void write_output(char* output, int result);
void sorting();
int subset_sum();
void get_ans();
void clear();

int num_element = 0;				//������ ������ ����
int L = 0;							//ã���� �ϴ� Subset Sum
int element[MAX_ELEMENT];			//������ ���Ҹ� ����
int ans[MAX_ELEMENT];				//Subset Sum�� �����ϴ� ���ҵ��� ����
int num_ans = 0;					//Subset Sum�� �����ϴ� ���ҵ��� ������ ����
int F[MAX_ELEMENT][MAX_ELEMENT];	//DP�� �����ϱ� ���� ���̺�

int main() {
	FILE* fp; int num_testcase = 0; int result = 0;
	char input[FILENAME_MAX]; char output[FILENAME_MAX];

	fp = fopen("SS_test_command.txt", "r");
	if (fp == NULL) {
		fprintf(stderr, "FILE OPEN ERROR");
		exit(1);
	}
	fscanf(fp, "%d\n", &num_testcase);
	for (int i = 0; i < num_testcase; i++) {
		fscanf(fp, "%s %s\n", input, output); //input file�� output file�� �̸��� ������

		clear();
		getinfo_element(input);

		result = subset_sum(); //TRUE or FALSE

		if (result == TRUE) {
			get_ans();
			sorting(); //����� ä���� ���� ascending�ϰ� sorting
		}

		write_output(output, result); //output file���� ����� ����
	}

	fclose(fp);
}

int subset_sum() {
	int i, j;
	F[0][0] = TRUE;
	for (j = 1; j <= L; j++) F[0][j] = FALSE;
	for (i = 1; i <= num_element; i++) {
		for (j = 0; j <= L; j++) {
			F[i][j] = F[i - 1][j];
			if (j - element[i] >= 0)
				F[i][j] = F[i][j] || F[i - 1][j - element[i]];
		}
	}

	return F[num_element][L];
}

void get_ans() {
	int j = L;
	for (int i = num_element; i >= 1; i--) {
		if (F[i][j] == FALSE) {
			fprintf(stderr, "Something wrong");
			exit(1);
		}

		if (F[i - 1][j] == TRUE) { //�ش� element�� subset sum�� �Ϻΰ� �ƴϴ�
			//do nothing in ans
			//do nothing in j
		}
		else {
			ans[num_ans++] = i - 1; //�ش� element�� subset sum�� �Ϻ��̹Ƿ� ans�� �ִ´�
			//(i�� 1���� n�����ε� ���� 0���� n-1�����̹Ƿ� 1�� ���ְ� ����)
			j = j - element[i]; //j�� ���� �Ѿ�� ��ŭ ����� �Ѵ�
		}
	}
}

void getinfo_element(char* input) {
	FILE* fp;

	//�Է� ���� input file�� ���
	fp = fopen(input, "r");
	if (fp == NULL) {
		fprintf(stderr, "FILE OPEN ERROR");
		exit(1);
	}

	fscanf(fp, "%d\n", &num_element); //element�� ������ ��������
	if (num_element >= MAX_ELEMENT) {
		fprintf(stderr, "Too much elements");
		exit(1);
	}

	element[0] = 0;
	for (int i = 1; i <= num_element; i++)
		fscanf(fp, "%d\n", &element[i]); //�� element�� ������� ����(��, 1���� n����)
	fscanf(fp, "%d", &L); //L���� ������ �ٿ��� ������

	fclose(fp);
}

void write_output(char* output, int result) {
	FILE* fp;

	fp = fopen(output, "w");
	if (fp == NULL) {
		fprintf(stderr, "FILE OPEN ERROR");
		exit(1);
	}

	fprintf(fp, "%d\n", result); //ù��° �ٿ� �κ� ������ �����ϴ��� ���θ� ���
	if (result == FALSE) { //�κ� ������ ��� 0�� ��������(FALSE)
		fclose(fp);	   //�״�� ������ �ݰ� ����
		return;
	}
	else { //�κ� ������ �־ 1�� ��������(TRUE)
		fprintf(fp, "%d\n", num_ans); //�ι�° �ٿ� �κ� ������ ������ ���� ����ϰ�
		for (int i = 0; i < num_ans; i++)  //�ε����� ���� �� ���´�
			fprintf(fp, "%d\n", ans[i]);

		fclose(fp);
		return;
	}
}

void sorting() { //selection sort�� ������� ����
	int temp;

	for (int i = 0; i < num_ans - 1; i++) {
		for (int j = i + 1; j < num_ans; j++) {
			if (ans[i] > ans[j]) { //swap
				temp = ans[i];
				ans[i] = ans[j];
				ans[j] = temp;
			}
		}
	}
}
void clear() {
	num_element = 0;				//������ ������ ����
	L = 0;							//ã���� �ϴ� Subset Sum
	num_ans = 0;					//Subset Sum�� �����ϴ� ���ҵ��� ������ ����
	for (int i = 0; i < MAX_ELEMENT; i++) {
		element[i] = 0;
		ans[i];
		for (int j = 0; j < MAX_ELEMENT; j++) {
			F[i][j] = 0;
		}
	}
}