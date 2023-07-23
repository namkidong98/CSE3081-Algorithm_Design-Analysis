#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#define FILENAME_MAX 260
#define MAX_VERTEX 201

double triangulation();
void getinfo_polygon(char* input);
void write_output(char* output, double result);
void sorting();
double distance(int v1, int v2);
void find_pairs(int f, int s);
void clear();

int num_vertex = 0; //input file�� ����� vertex�� ����
typedef struct pos {
	double x;
	double y;
}pos;
pos vertex[MAX_VERTEX]; //input file�� polygon ������ �����ϴ� ����
//�������� ��ǥ�� �ð�������� ���������� ����

typedef struct pair {
	int left;
	int right;
}pair;
pair ans[MAX_VERTEX];	//Minimal_Triangulation���� ã�� �������� ������ ������ ����(����)
int pair_cnt = 0;

int k_table[MAX_VERTEX][MAX_VERTEX];
double table[MAX_VERTEX][MAX_VERTEX]; //minimum cost of triangulating�� ������ �����ϴ� ���̺�

int main() {
	FILE* fp; int num_testcase = 0;
	char input[FILENAME_MAX]; char output[FILENAME_MAX];
	double result = 0;

	fp = fopen("PT_test_command.txt", "r");
	fscanf(fp, "%d\n", &num_testcase);
	for (int i = 0; i < num_testcase; i++) {
		fscanf(fp, "%s %s\n", input, output); //input file�� output file�� �̸��� ������

		clear();
		getinfo_polygon(input); //input file���� polygon�� ������ �о��

		//�ﰢȭ ����
		result = triangulation();
		find_pairs(0, num_vertex);
		sorting(); //ans�� ���ؼ� left �켱 asc, ������ left�� ���� right asc���� ����

		write_output(output, result); //output file���� ����� ����
	}

	fclose(fp);
}

double triangulation() {
	int i, s, k; //i�� ���� vertex�� index, s�� ����� ����
	double min, result;

	for (s = 0; s < num_vertex + 1; s++) {
		for (i = 0; i < num_vertex + 1; i++) {
			if (s < 4) table[s][i] = 0; // if 0 <= s < 4
			else {						// if s >= 4
				min = DBL_MAX; //double�� �ִ��� min���� �����صΰ�
				for (k = 1; k <= s - 2; k++) {
					result = table[k + 1][i] + table[s - k][i + k]
						+ distance(i, i + k) + distance(i + k, i + s - 1);

					if (result < min) {
						min = result; //�ش� ������ ����� ���� min���� ������ min ��ü
						k_table[s][i] = k;
					}
				}

				table[s][i] = min; //�������� ������ �ּҺ������ �ش� �ε����� table ä���
			}
		}
	}

	return table[num_vertex][0];
}

void find_pairs(int i, int s) {
	if (s < 4) return;

	int k = k_table[s][i];

	if (k > 1) { //�� ���ึ�� ���� ������ 1������ �ּҷ� �߻��Ѵ�
		ans[pair_cnt].left = i;
		ans[pair_cnt++].right = i + k;
	}
	if (s - k > 2) { //�������� ���� ��쿡�� pair�� �� �� �� �߻��Ѵ�
		ans[pair_cnt].left = i + k;
		ans[pair_cnt++].right = i + s - 1;
	}

	find_pairs(i, k + 1);
	find_pairs(i + k, s - k);
}
double distance(int v1, int v2) {
	int diff = abs(v1 - v2); //�� vertex�� index�� ������ ������ ���ϰ�

	if (diff == 1 || diff == num_vertex - 1) return 0.0; //�� vertex�� adjacent�ϸ�, 0�� ��ȯ

	else { //�� vertex�� adjacent���� ������, euclidean distance�� ��ȯ
		double x = vertex[v1].x - vertex[v2].x;
		double y = vertex[v1].y - vertex[v2].y;
		return sqrt(x * x + y * y);
	}
}
void getinfo_polygon(char* input) {
	FILE* fp;

	//�Է� ���� input file�� ���
	fp = fopen(input, "r");
	fscanf(fp, "%d\n", &num_vertex); //vertex�� ������ �Է� ��������
	if (num_vertex >= MAX_VERTEX) {
		fprintf(stderr, "Too much vertex");
		exit(1);
	}

	for (int i = 0; i < num_vertex; i++)
		fscanf(fp, "%lf %lf\n", &vertex[i].x, &vertex[i].y); //�� vertex�� x, y��ǥ�� ����

	fclose(fp);
}
void write_output(char* output, double result) {
	FILE* fp;

	fp = fopen(output, "w");
	fprintf(fp, "%lf\n", result);

	for (int i = 0; i < num_vertex - 3; i++)  //n���� vertex���� n-3���� ���� ���´�
		fprintf(fp, "%d %d\n", ans[i].left, ans[i].right);

	fclose(fp);
}
void sorting() { //selection sort�� ������� ����
	int end = num_vertex - 3; //������ �Ǵ� pair�� n-3���� ������ �̷���� �ִ�
	pair temp;

	//left�� �������� asceding�ϰ� sorting
	for (int i = 0; i < end - 1; i++) {
		for (int j = i + 1; j < end; j++) {
			if (ans[i].left > ans[j].left) {
				//left�� swap
				temp.left = ans[i].left;
				ans[i].left = ans[j].left;
				ans[j].left = temp.left;

				//right�� swap
				temp.right = ans[i].right;
				ans[i].right = ans[j].right;
				ans[j].right = temp.right;
			}

			else if (ans[i].left == ans[j].left) { //������ left�� ���� ���
				if (ans[i].right > ans[j].right) { //right�� �������� ascending�ϰ� sorting
					//left�� swap
					temp.left = ans[i].left;
					ans[i].left = ans[j].left;
					ans[j].left = temp.left;

					//right�� swap
					temp.right = ans[i].right;
					ans[i].right = ans[j].right;
					ans[j].right = temp.right;
				}
			}
		}
	}

}
void clear() {
	pair_cnt = 0;
	num_vertex = 0;
	for (int i = 0; i < MAX_VERTEX; i++) {
		ans[i].left = 0; ans[i].right = 0;
		for (int j = 0; j < MAX_VERTEX; j++) {
			k_table[i][j] = 0;
			table[i][j] = 0.0;
		}
	}
}