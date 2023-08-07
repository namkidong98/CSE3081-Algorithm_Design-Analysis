#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Edge* edge_pointer;
typedef struct Edge {
	int from;
	int to;
	int weight;
}edge;

typedef struct Graph* graph_pointer;
typedef struct Graph {
	int num_vertex;
	int num_edge;
	edge_pointer edge;
}graph;

typedef struct Subset {
	int parent;
	int rank;
}subset;

int MAX_WEIGHT;			//ASCII ���Ͽ��� �о��, edge�� weight�� ���� ���� �� �ִ� ���� ū ��
edge_pointer ans;		//����� �Ǵ� edge�� �����ϴ� edge_pointer
int ans_cnt = 0;		//����� �Ǵ� edge�� index���� �� ����

graph_pointer createGraph(int num_vertex, int num_edge);	//Vertex�� ������ Edge�� ������ ���� Graph�� ����� �����͸� ��ȯ�ϴ� �Լ�
int find(subset subsets[], int i);							//�ش� ������ parent�� ã�Ƽ� ��ȯ�ϴ� �Լ�
void Union(subset subsets[], int x, int y);					//�� ���Ұ� ���� ������ ����(merge)�ϴ� �Լ�
int myComp(const void* a, const void* b);					//qsort�� ���� ������ ������ �Ǵ� �� �Լ�
void KruskalMST(graph* graph);								//disjoint set�� find, union�� �������� ������ MST�� ���ϴ� Kruskal �˰���

int main() {
	FILE* fp;
	int n_vertex;			//ASCII ���Ͽ��� �о��, vertex�� ����
	int n_edge;				//ASCII ���Ͽ��� �о��, edge�� ����

	//fp = fopen("example.txt", "r");
	fp = fopen("HW3_email-Eu-core.txt", "r");
	if (fp == NULL) {
		fprintf(stderr, "FILE OPEN ERROR!\n");
		exit(0);
	}

	fscanf(fp, "%d %d %d", &n_vertex, &n_edge, &MAX_WEIGHT); //������ ù �ٿ��� vertex�� ����, edge�� ����, �ִ� weight�� �Է� �޴´�
	graph_pointer graph = createGraph(n_vertex, n_edge); //edge�� ������ŭ �޸𸮸� �Ҵ��Ͽ� graph�� �����
	for (int i = 0; i < n_edge; i++)  //������ ��° �ٺ��� edge�� ������ŭ edge�� ������ �Է� �޴´�
		fscanf(fp, "%d %d %d", &graph->edge[i].from, &graph->edge[i].to, &graph->edge[i].weight);

	KruskalMST(graph);

	return 0;
}

graph_pointer createGraph(int num_vertex, int num_edge) { //Vertex�� ������ Edge�� ������ ���� Graph�� ����� �����͸� ��ȯ�ϴ� �Լ�
	graph_pointer temp = (graph_pointer)malloc(sizeof(graph));
	temp->num_vertex = num_vertex;
	temp->num_edge = num_edge;

	//edge�� ������ŭ ������ ������ ������ �Ҵ��ϰ� edge_pointer�� ����
	temp->edge = (edge_pointer)malloc(num_edge * sizeof(edge));

	return temp;
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
int myComp(const void* a, const void* b) { //qsort�� ���� ������ ������ �Ǵ� �� �Լ�
	edge_pointer a1 = (edge_pointer)a;
	edge_pointer b1 = (edge_pointer)b;
	return a1->weight - b1->weight; //edge�� weight�� �������� �������� ����(�տ� ���� �� ũ�� ���� ��ȯ --> SWAP ����)
}
void KruskalMST(graph* graph) {
	ans = (edge_pointer)malloc((graph->num_vertex - 1) * sizeof(edge));

	int cnt = 0; //edge�� ������ �˻��ϴµ� ����ϴ� �ε���

	//1�ܰ�: Edge���� weight�� �������� �������� �����Ѵ�
	qsort(graph->edge, graph->num_edge, sizeof(graph->edge[0]), myComp);

	subset* subsets = (subset*)malloc(graph->num_vertex * sizeof(subset)); //Vertex ������ŭ set�� ������ �Ҵ�(single node�� ����� ����)
	for (int i = 0; i < graph->num_vertex; i++) { //V subset�� single node�� �����
		subsets[i].parent = i; //single node�� �ڱ� �����ΰ� �ڽ��� parent�̴�
		subsets[i].rank = 0; //rank�� root�κ��� �� depth�� �ǹ��ϹǷ� root node, single node�� rank�� 0�̴�
	}

	//edge�� n_vertex-1�� ���õǾ�� �Ѵ�(Minimal Spanning Tree�� ����)
	while (ans_cnt < graph->num_vertex - 1) {
		//2�ܰ�: ���� ���� edge�� ���� ���� ������ ���� index�� ������Ų��(Greedy)
		edge next_edge = graph->edge[cnt++];

		//3�ܰ�: ������ edge�� ��� vertex�� ���� vertex�� ������ ������ find�� ���� �˻��Ѵ�
		printf("next_edge[%d]: %-5d %-5d\n", cnt, next_edge.from, next_edge.to);
		int x = find(subsets, next_edge.from); //next_edge�� ��� vertex�� ������ ������ root�� x�� �޾ƿ�
		int y = find(subsets, next_edge.to);   //next_edge�� ���� vertex�� ������ ������ root�� y�� �޾ƿ�

		
		//4-1�ܰ�: ���� �ش� edge(next_edge)�� cycle�� �ʷ����� ������ �����Ѵ�
		if (x != y) { //edge�� ��� vertex�� ���� vertex�� ���� ������ �ƴϾ��ٴ� �� --> edge�� �����ص� cycle�� �߻����� �ʴ´�
			ans[ans_cnt++] = next_edge;
			Union(subsets, x, y);
		}
		//4-2�ܰ�: �ش� edge(next_edge)�� cycle�� �ʷ��ϸ� edge�� ������
		else {}; //do nothing
		
	}
	//(�߿�) ���⼭ �Է����� �־��� ���� �ϳ��� connected graph�̶�� ������ ��� �ִ�. ���� connected���� ������ ������ �߻�

	//����� ����Ѵ�
	printf("Following are the edges in the constructed MST\n");
	for (int i = 0; i < ans_cnt; i++) {
		printf("%d -- %d == %d\n", ans[i].from, ans[i].to, ans[i].weight);
	}
}
