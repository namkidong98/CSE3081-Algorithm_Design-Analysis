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

int num_element = 0;				//집합의 원소의 개수
int L = 0;							//찾고자 하는 Subset Sum
int element[MAX_ELEMENT];			//집합의 원소를 저장
int ans[MAX_ELEMENT];				//Subset Sum을 만족하는 원소들의 집합
int num_ans = 0;					//Subset Sum을 만족하는 원소들의 집합의 개수
int F[MAX_ELEMENT][MAX_ELEMENT];	//DP를 구현하기 위한 테이블

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
		fscanf(fp, "%s %s\n", input, output); //input file과 output file의 이름을 가져옴

		clear();
		getinfo_element(input);

		result = subset_sum(); //TRUE or FALSE

		if (result == TRUE) {
			get_ans();
			sorting(); //기계적 채점을 위해 ascending하게 sorting
		}

		write_output(output, result); //output file에서 결과를 저장
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

		if (F[i - 1][j] == TRUE) { //해당 element는 subset sum의 일부가 아니다
			//do nothing in ans
			//do nothing in j
		}
		else {
			ans[num_ans++] = i - 1; //해당 element는 subset sum의 일부이므로 ans에 넣는다
			//(i는 1부터 n까지인데 답은 0부터 n-1까지이므로 1을 빼주고 저장)
			j = j - element[i]; //j의 값이 넘어온 만큼 빼줘야 한다
		}
	}
}

void getinfo_element(char* input) {
	FILE* fp;

	//입력 받은 input file을 열어서
	fp = fopen(input, "r");
	if (fp == NULL) {
		fprintf(stderr, "FILE OPEN ERROR");
		exit(1);
	}

	fscanf(fp, "%d\n", &num_element); //element의 개수를 가져오고
	if (num_element >= MAX_ELEMENT) {
		fprintf(stderr, "Too much elements");
		exit(1);
	}

	element[0] = 0;
	for (int i = 1; i <= num_element; i++)
		fscanf(fp, "%d\n", &element[i]); //각 element를 순서대로 저장(단, 1부터 n까지)
	fscanf(fp, "%d", &L); //L값을 마지막 줄에서 가져옴

	fclose(fp);
}

void write_output(char* output, int result) {
	FILE* fp;

	fp = fopen(output, "w");
	if (fp == NULL) {
		fprintf(stderr, "FILE OPEN ERROR");
		exit(1);
	}

	fprintf(fp, "%d\n", result); //첫번째 줄에 부분 집합이 존재하는지 여부를 출력
	if (result == FALSE) { //부분 집합이 없어서 0을 적었으면(FALSE)
		fclose(fp);	   //그대로 파일을 닫고 리턴
		return;
	}
	else { //부분 집합이 있어서 1을 적었으면(TRUE)
		fprintf(fp, "%d\n", num_ans); //두번째 줄에 부분 집합의 원소의 개수 출력하고
		for (int i = 0; i < num_ans; i++)  //인덱스가 한줄 씩 나온다
			fprintf(fp, "%d\n", ans[i]);

		fclose(fp);
		return;
	}
}

void sorting() { //selection sort를 기반으로 정렬
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
	num_element = 0;				//집합의 원소의 개수
	L = 0;							//찾고자 하는 Subset Sum
	num_ans = 0;					//Subset Sum을 만족하는 원소들의 집합의 개수
	for (int i = 0; i < MAX_ELEMENT; i++) {
		element[i] = 0;
		ans[i];
		for (int j = 0; j < MAX_ELEMENT; j++) {
			F[i][j] = 0;
		}
	}
}