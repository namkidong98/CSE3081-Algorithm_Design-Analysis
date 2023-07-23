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

int num_vertex = 0; //input file에 저장된 vertex의 개수
typedef struct pos {
	double x;
	double y;
}pos;
pos vertex[MAX_VERTEX]; //input file의 polygon 정보를 저장하는 변수
//꼭지점의 좌표는 시계방향으로 순차적으로 나열

typedef struct pair {
	int left;
	int right;
}pair;
pair ans[MAX_VERTEX];	//Minimal_Triangulation으로 찾은 꼭짓점의 정보를 저장한 변수(정답)
int pair_cnt = 0;

int k_table[MAX_VERTEX][MAX_VERTEX];
double table[MAX_VERTEX][MAX_VERTEX]; //minimum cost of triangulating의 정보를 저장하는 테이블

int main() {
	FILE* fp; int num_testcase = 0;
	char input[FILENAME_MAX]; char output[FILENAME_MAX];
	double result = 0;

	fp = fopen("PT_test_command.txt", "r");
	fscanf(fp, "%d\n", &num_testcase);
	for (int i = 0; i < num_testcase; i++) {
		fscanf(fp, "%s %s\n", input, output); //input file과 output file의 이름을 가져옴

		clear();
		getinfo_polygon(input); //input file에서 polygon의 정보를 읽어옴

		//삼각화 진행
		result = triangulation();
		find_pairs(0, num_vertex);
		sorting(); //ans에 대해서 left 우선 asc, 동일한 left에 대해 right asc으로 정렬

		write_output(output, result); //output file에서 결과를 저장
	}

	fclose(fp);
}

double triangulation() {
	int i, s, k; //i는 현재 vertex의 index, s는 노드의 개수
	double min, result;

	for (s = 0; s < num_vertex + 1; s++) {
		for (i = 0; i < num_vertex + 1; i++) {
			if (s < 4) table[s][i] = 0; // if 0 <= s < 4
			else {						// if s >= 4
				min = DBL_MAX; //double의 최댓값을 min으로 설정해두고
				for (k = 1; k <= s - 2; k++) {
					result = table[k + 1][i] + table[s - k][i + k]
						+ distance(i, i + k) + distance(i + k, i + s - 1);

					if (result < min) {
						min = result; //해당 시행의 결과가 기존 min보다 작으면 min 대체
						k_table[s][i] = k;
					}
				}

				table[s][i] = min; //최종으로 결정된 최소비용으로 해당 인덱스의 table 채우기
			}
		}
	}

	return table[num_vertex][0];
}

void find_pairs(int i, int s) {
	if (s < 4) return;

	int k = k_table[s][i];

	if (k > 1) { //매 실행마다 선은 무조건 1개씩은 최소로 발생한다
		ans[pair_cnt].left = i;
		ans[pair_cnt++].right = i + k;
	}
	if (s - k > 2) { //인접하지 않은 경우에는 pair가 한 개 더 발생한다
		ans[pair_cnt].left = i + k;
		ans[pair_cnt++].right = i + s - 1;
	}

	find_pairs(i, k + 1);
	find_pairs(i + k, s - k);
}
double distance(int v1, int v2) {
	int diff = abs(v1 - v2); //두 vertex의 index의 차이의 절댓값을 구하고

	if (diff == 1 || diff == num_vertex - 1) return 0.0; //두 vertex가 adjacent하면, 0을 반환

	else { //두 vertex가 adjacent하지 않으면, euclidean distance을 반환
		double x = vertex[v1].x - vertex[v2].x;
		double y = vertex[v1].y - vertex[v2].y;
		return sqrt(x * x + y * y);
	}
}
void getinfo_polygon(char* input) {
	FILE* fp;

	//입력 받은 input file을 열어서
	fp = fopen(input, "r");
	fscanf(fp, "%d\n", &num_vertex); //vertex의 개수를 입력 가져오고
	if (num_vertex >= MAX_VERTEX) {
		fprintf(stderr, "Too much vertex");
		exit(1);
	}

	for (int i = 0; i < num_vertex; i++)
		fscanf(fp, "%lf %lf\n", &vertex[i].x, &vertex[i].y); //각 vertex의 x, y좌표를 저장

	fclose(fp);
}
void write_output(char* output, double result) {
	FILE* fp;

	fp = fopen(output, "w");
	fprintf(fp, "%lf\n", result);

	for (int i = 0; i < num_vertex - 3; i++)  //n개의 vertex에서 n-3개의 쌍이 나온다
		fprintf(fp, "%d %d\n", ans[i].left, ans[i].right);

	fclose(fp);
}
void sorting() { //selection sort를 기반으로 정렬
	int end = num_vertex - 3; //정답이 되는 pair은 n-3개의 쌍으로 이루어져 있다
	pair temp;

	//left를 기준으로 asceding하게 sorting
	for (int i = 0; i < end - 1; i++) {
		for (int j = i + 1; j < end; j++) {
			if (ans[i].left > ans[j].left) {
				//left쪽 swap
				temp.left = ans[i].left;
				ans[i].left = ans[j].left;
				ans[j].left = temp.left;

				//right쪽 swap
				temp.right = ans[i].right;
				ans[i].right = ans[j].right;
				ans[j].right = temp.right;
			}

			else if (ans[i].left == ans[j].left) { //동일한 left를 가진 경우
				if (ans[i].right > ans[j].right) { //right를 기준으로 ascending하게 sorting
					//left쪽 swap
					temp.left = ans[i].left;
					ans[i].left = ans[j].left;
					ans[j].left = temp.left;

					//right쪽 swap
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