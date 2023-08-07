#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Edge* edge_pointer;
typedef struct Edge {
	int from;
	int to;
	int weight;
}edge;
typedef struct Subset {
	int parent;
	int rank;
	int root;
}subset;
typedef struct Group {
	int root;				//�ش� group�� root
	int comp_cnt;			//�ش� group�� ���� component�� ����
	long long weight_total;	//�ش� group�� MST�� �����ϴ� weight�� ����
}Group;

int n_vertex;			//ASCII ���Ͽ��� �о��, vertex�� ����
int n_edge;				//ASCII ���Ͽ��� �о��, edge�� ����
int MAX_WEIGHT;			//ASCII ���Ͽ��� �о��, edge�� weight�� ���� ���� �� �ִ� ���� ū ��
edge_pointer Edge_Arr;	//Edge�� ������ �����ϴ� �迭
Group* group;			//connected graph ������ �����ϴ� �迭
int group_cnt = 0;		//connected graph�� ����
int k_scanned = 0;		//MST�� ������ ������ ���� edge�� ����
clock_t start, end;		//�ð��� ������ �� ����ϴ� ����

int find(subset subsets[], int i);				//�ش� ������ parent�� ã�Ƽ� ��ȯ�ϴ� �Լ�
void Union(subset subsets[], int x, int y);		//�� ���Ұ� ���� ������ ����(merge)�ϴ� �Լ�
void KruskalMST();								//disjoint set�� find, union�� �������� ������ MST�� ���ϴ� Kruskal �˰���
void buildMinHeap(edge_pointer arr, int n);		//Minheap�� ����� �Լ�
void heapify(edge_pointer arr, int n, int i);	//heap�� ������ �����ϵ��� �ϴ� �Լ�
edge extractMin(edge_pointer arr, int* n);		//���� ���� root node�� min heap���� ������ �Լ�

int main() {
	FILE* fp;
	char input[1000], output[100], temp[1000];

	fp = fopen("command.txt", "r");
	if (fp == NULL) {
		fprintf(stderr, "FILE OPEN ERROR!\n");
		exit(0);
	}
	fgets(input, 1000, fp);
	int i = 0;
	while (input[i] != '\n') i++;
	input[i] = '\\';
	strcpy(output, input); //output���� ����

	//input file�� ���� �ּ� �����
	fscanf(fp, "%s", temp);
	strcat(input, "\\");
	strcat(input, temp);
	fscanf(fp, "%s", output);

	fclose(fp);

	fp = fopen(input, "r");
	if (fp == NULL) {
		fprintf(stderr, "FILE OPEN ERROR!\n");
		exit(0);
	}

	fscanf(fp, "%d %d %d", &n_vertex, &n_edge, &MAX_WEIGHT); //������ ù �ٿ��� vertex�� ����, edge�� ����, �ִ� weight�� �Է� �޴´�
	Edge_Arr = (edge_pointer)malloc(sizeof(edge) * n_edge);
	for (int i = 0; i < n_edge; i++) { //������ ��° �ٺ��� edge�� ������ŭ edge�� ������ �Է� �޴´�
		fscanf(fp, "%d %d %d", &Edge_Arr[i].from, &Edge_Arr[i].to, &Edge_Arr[i].weight);
		if (Edge_Arr[i].weight > MAX_WEIGHT || Edge_Arr[i].from > n_vertex || Edge_Arr[i].to > n_vertex) {
			fprintf(stderr, "DATA FROM FILE HAS ERROR\n");
			exit(0);
		}
	}
	fclose(fp);
	printf("%d %d\n", n_vertex, n_edge);

	start = clock();
	KruskalMST();
	end = clock();
	double total_time = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("Time spent: %f seconds\n", total_time);

	fp = fopen(output, "w");
	if (fp == NULL) {
		fprintf(stderr, "FILE MAKE ERROR!\n");
		exit(0);
	}
	fprintf(fp, "%d\n", group_cnt);

	for (int i = 0; i < group_cnt; i++) {
		fprintf(fp, "%d %lld\n", group[i].comp_cnt, group[i].weight_total);
	}
	fclose(fp);

	return 0;
}
int find(subset subsets[], int i) { //�ش� ������ parent�� ã�Ƽ� ��ȯ�ϴ� �Լ�
	if (subsets[i].parent != i) //parent�� �ڱ� �ڽ��� �ƴ� ��� parent�� ã�� ������ recursion ����
		subsets[i].parent = find(subsets, subsets[i].parent);

	return subsets[i].parent;
}
void Union(subset subsets[], int x, int y) { //�� ���Ұ� ���� ������ ����(merge)�ϴ� �Լ�
	int xroot = find(subsets, x); //x�� ���� ������ root�� ã��
	int yroot = find(subsets, y); //y�� ���� ������ root�� ã��

	//rank�� ū ������ merge(rank�� ���� ���� root�� parent�� ū ���� root�� ����)
	if (subsets[xroot].rank < subsets[yroot].rank)
		subsets[xroot].parent = yroot;
	else if (subsets[xroot].rank > subsets[yroot].rank)
		subsets[yroot].parent = xroot;

	//rank�� �����ϸ� �� ������ merge�ϰ� ������ rank�� 1 ����
	else {
		subsets[yroot].parent = xroot;
		subsets[xroot].rank++;
	}
}
void KruskalMST() {
	int temp;
	group = (Group*)malloc(sizeof(Group) * n_vertex);
	subset* subsets = (subset*)malloc(n_vertex * sizeof(subset)); //Vertex ������ŭ set�� ������ �Ҵ�(single node�� ����� ����)
	for (int i = 0; i < n_vertex; i++) { //V subset�� single node�� �����
		subsets[i].parent = i; //single node�� �ڱ� �����ΰ� �ڽ��� parent�̴�
		subsets[i].rank = 0; //rank�� root�κ��� �� depth�� �ǹ��ϹǷ� root node, single node�� rank�� 0�̴�
	}

	//Union-Find�� connected graph�� �����
	for (int i = 0; i < n_edge; i++) {
		edge next_edge = Edge_Arr[i];
		int x = find(subsets, next_edge.from); //next_edge�� ��� vertex�� ������ ������ root�� x�� �޾ƿ�
		int y = find(subsets, next_edge.to);   //next_edge�� ���� vertex�� ������ ������ root�� y�� �޾ƿ�
		if (x != y)
			Union(subsets, x, y);
	}

	//��� vertex�� ��� �˻��ϸ鼭 connected graph�� �� �� �ִ���, �ش� graph�� ������ ��Ÿ���� root�� �������� ����
	for (int i = 0; i < n_vertex; i++) {
		if (i == find(subsets, i)) {
			group[group_cnt].root = i;			//�ش� connected group�� root�� ����
			group[group_cnt].weight_total = 0;	//�ʱ�ȭ
			group[group_cnt].comp_cnt = 0;		//�ʱ�ȭ
			group_cnt++;						//connected group�� ������ 1 ���� 
		}
	}

	//connected graph(group)�� component ������ ����ϰ� subset�� root�� ����Ͽ� �����س��´�
	for (int i = 0; i < n_vertex; i++) {
		temp = find(subsets, i);
		for (int j = 0; j < group_cnt; j++) {
			if (temp == group[j].root) { //connected graph(group)�� �� root�� ���� ������ ã����
				group[j].comp_cnt++;	 //�ش� graph�� connected component�� ���� ������Ų��
				subsets[i].root = group[j].root;
				break;
			}
		}
	}

	int num_edge = n_edge;
	int in_edge = 0;
	buildMinHeap(Edge_Arr, n_edge);

	//subsets�� root���� �ʱ�ȭ ���Ѽ� �ٽ� find-union�� �� �� �ִ� ���·� �����
	for (int i = 0; i < n_vertex; i++) {
		subsets[i].parent = i;
		subsets[i].rank = 0;
	}

	//��� Edge�� �˻��ϸ鼭 
	while (num_edge > 0) {
		edge next_edge = extractMin(Edge_Arr, &num_edge);
		k_scanned++; //heap�� ���Ұ� �ϳ��� ���� �� ���� �����༭ 

		//edge�� ��� vertex�� ���� vertex�� ������ ������ find�� ���� �˻��Ѵ�
		int x = find(subsets, next_edge.from); //next_edge�� ��� vertex�� ������ ������ root�� x�� �޾ƿ�
		int y = find(subsets, next_edge.to);   //next_edge�� ���� vertex�� ������ ������ root�� y�� �޾ƿ�

		//���� �ش� edge(next_edge)�� cycle�� �ʷ����� ������ �����Ѵ�
		//x != y : edge�� ��� vertex�� ���� vertex�� ���� ������ �ƴϾ��ٴ� �� --> edge�� �����ص� cycle�� �߻����� �ʴ´�
		//subsets[next_edge.from].root == subsets[next_edge.to].root : ���� ���� ����, �� ���� connected graph�� ������ ��
		if (x != y && subsets[x].root == subsets[y].root) {
			in_edge++;
			if (in_edge == n_vertex - 1)
				break;
			temp = subsets[x].root; //connected graph�� root�� temp�� �����ϰ�
			for (int j = 0; j < group_cnt; j++) { //connected graph�� ����� ã�ƺ��鼭
				if (temp == group[j].root) { // �ش��ϴ� connected graph�� ã����
					group[j].weight_total += next_edge.weight;	 //�ش� graph�� total weight�� ������Ų��
					break;
				}
			}
			Union(subsets, x, y);
		}
	}
	printf("k_scanned: %d   in edge: %d    n_vertex: %d\n", k_scanned, in_edge, n_vertex);
}
void buildMinHeap(edge_pointer arr, int n) { // �迭 ������� min heap ����
	for (int i = n / 2 - 1; i >= 0; i--) { // ������ ���� ������ �������� heapify ����
		heapify(arr, n, i);
	}
}
void heapify(edge_pointer arr, int n, int i) {
	int smallest = i;  // ���� ��带 ���� ���� ������ ����
	int left = 2 * i + 1;  // ���� �ڽ� ��� �ε���
	int right = 2 * i + 2;  // ������ �ڽ� ��� �ε���
	int tmp_weight, tmp_to, tmp_from;

	// ���� �ڽ��� ��Ʈ���� ���� ���
	if (left < n && arr[left].weight < arr[smallest].weight) smallest = left;

	// ������ �ڽ��� ��Ʈ���� ���� ���
	if (right < n && arr[right].weight < arr[smallest].weight) smallest = right;

	// ���� ��Ʈ�� ���� ���� ���� ��ȯ
	if (smallest != i) {
		tmp_weight = arr[i].weight; tmp_from = arr[i].from; tmp_to = arr[i].to;
		arr[i].weight = arr[smallest].weight; arr[i].from = arr[smallest].from; arr[i].to = arr[smallest].to;
		arr[smallest].weight = tmp_weight; arr[smallest].from = tmp_from; arr[smallest].to = tmp_to;

		// ����� �ڽ� ��忡 ���� ��������� heapify ȣ��
		heapify(arr, n, smallest);
	}
}
edge extractMin(edge_pointer arr, int* n) { // min heap���� �ּҰ��� ������ �Լ�
	if (*n <= 0) {
		fprintf(stderr, "HEAP IS Empty!\n");
		exit(0);  // ���� ������� ��� -1 ��ȯ �Ǵ� ���� ó��
	}
	edge min_edge = arr[0];  // �ּҰ��� ��Ʈ ��忡 ��ġ
	arr[0] = arr[*n - 1];  // ���� ������ ��带 ��Ʈ�� �̵�
	(*n)--;  // ���� ũ�⸦ 1 ����

	heapify(arr, *n, 0);  // �� �Ӽ� ������ ���� heapify ȣ��

	return min_edge;
}
