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

int MAX_WEIGHT;			//ASCII 파일에서 읽어온, edge의 weight가 가장 가질 수 있는 가장 큰 값
edge_pointer ans;		//결과가 되는 edge를 저장하는 edge_pointer
int ans_cnt = 0;		//결과가 되는 edge의 index이자 총 개수

graph_pointer createGraph(int num_vertex, int num_edge);	//Vertex의 개수와 Edge의 개수를 만들어서 Graph를 만들어 포인터를 반환하는 함수
int find(subset subsets[], int i);							//해당 원소의 parent를 찾아서 반환하는 함수
void Union(subset subsets[], int x, int y);					//두 원소가 속한 집합을 병합(merge)하는 함수
int myComp(const void* a, const void* b);					//qsort에 쓰일 정렬의 기준이 되는 비교 함수
void KruskalMST(graph* graph);								//disjoint set의 find, union을 바탕으로 구현한 MST를 구하는 Kruskal 알고리즘

int main() {
	FILE* fp;
	int n_vertex;			//ASCII 파일에서 읽어온, vertex의 개수
	int n_edge;				//ASCII 파일에서 읽어온, edge의 개수

	//fp = fopen("example.txt", "r");
	fp = fopen("HW3_email-Eu-core.txt", "r");
	if (fp == NULL) {
		fprintf(stderr, "FILE OPEN ERROR!\n");
		exit(0);
	}

	fscanf(fp, "%d %d %d", &n_vertex, &n_edge, &MAX_WEIGHT); //파일의 첫 줄에서 vertex의 개수, edge의 개수, 최대 weight를 입력 받는다
	graph_pointer graph = createGraph(n_vertex, n_edge); //edge의 개수만큼 메모리를 할당하여 graph를 만든다
	for (int i = 0; i < n_edge; i++)  //파일의 둘째 줄부터 edge의 개수만큼 edge의 정보를 입력 받는다
		fscanf(fp, "%d %d %d", &graph->edge[i].from, &graph->edge[i].to, &graph->edge[i].weight);

	KruskalMST(graph);

	return 0;
}

graph_pointer createGraph(int num_vertex, int num_edge) { //Vertex의 개수와 Edge의 개수를 만들어서 Graph를 만들어 포인터를 반환하는 함수
	graph_pointer temp = (graph_pointer)malloc(sizeof(graph));
	temp->num_vertex = num_vertex;
	temp->num_edge = num_edge;

	//edge의 개수만큼 정보를 저장할 공간을 할당하고 edge_pointer로 접근
	temp->edge = (edge_pointer)malloc(num_edge * sizeof(edge));

	return temp;
}
int find(subset subsets[], int i) { //해당 원소의 parent를 찾아서 반환하는 함수
	if (subsets[i].parent != i) //parent가 자기 자신이 아닌 경우 parent를 찾을 때까지 recursion 수행
		subsets[i].parent = find(subsets, subsets[i].parent);

	return subsets[i].parent;
}
void Union(subset subsets[], int x, int y) { //두 원소가 속한 집합을 병합(merge)하는 함수
	int xroot = find(subsets, x); //x가 속한 집합의 root를 찾고
	int yroot = find(subsets, y); //y가 속한 집합의 root를 찾아

	//rank가 큰 쪽으로 merge(rank가 작은 쪽의 root의 parent를 큰 쪽의 root로 설정)
	if (subsets[xroot].rank < subsets[yroot].rank)
		subsets[xroot].parent = yroot;
	else if (subsets[xroot].rank > subsets[yroot].rank)
		subsets[yroot].parent = xroot;

	//rank가 동일하면 한 쪽으로 merge하고 한쪽의 rank를 1 증가
	else {
		subsets[yroot].parent = xroot;
		subsets[xroot].rank++;
	}
}
int myComp(const void* a, const void* b) { //qsort에 쓰일 정렬의 기준이 되는 비교 함수
	edge_pointer a1 = (edge_pointer)a;
	edge_pointer b1 = (edge_pointer)b;
	return a1->weight - b1->weight; //edge의 weight를 기준으로 오름차순 정렬(앞에 것이 더 크면 참을 반환 --> SWAP 진행)
}
void KruskalMST(graph* graph) {
	ans = (edge_pointer)malloc((graph->num_vertex - 1) * sizeof(edge));

	int cnt = 0; //edge의 모음을 검사하는데 사용하는 인덱스

	//1단계: Edge들을 weight를 기준으로 오름차순 정렬한다
	qsort(graph->edge, graph->num_edge, sizeof(graph->edge[0]), myComp);

	subset* subsets = (subset*)malloc(graph->num_vertex * sizeof(subset)); //Vertex 개수만큼 set의 공간을 할당(single node를 만들기 위해)
	for (int i = 0; i < graph->num_vertex; i++) { //V subset을 single node로 만든다
		subsets[i].parent = i; //single node는 자기 스스로가 자신의 parent이다
		subsets[i].rank = 0; //rank는 root로부터 들어간 depth를 의미하므로 root node, single node의 rank는 0이다
	}

	//edge는 n_vertex-1개 선택되어야 한다(Minimal Spanning Tree의 정의)
	while (ans_cnt < graph->num_vertex - 1) {
		//2단계: 가장 작은 edge를 고르고 다음 실행을 위해 index를 증가시킨다(Greedy)
		edge next_edge = graph->edge[cnt++];

		//3단계: 선택한 edge의 출발 vertex와 도착 vertex의 집합이 같은지 find를 통해 검사한다
		printf("next_edge[%d]: %-5d %-5d\n", cnt, next_edge.from, next_edge.to);
		int x = find(subsets, next_edge.from); //next_edge의 출발 vertex를 포함한 집합의 root를 x로 받아옴
		int y = find(subsets, next_edge.to);   //next_edge의 도착 vertex를 포함한 집합의 root를 y로 받아옴

		
		//4-1단계: 만약 해당 edge(next_edge)가 cycle을 초래하지 않으면 포함한다
		if (x != y) { //edge의 출발 vertex와 도착 vertex가 같은 집합이 아니었다는 뜻 --> edge를 포함해도 cycle이 발생하지 않는다
			ans[ans_cnt++] = next_edge;
			Union(subsets, x, y);
		}
		//4-2단계: 해당 edge(next_edge)가 cycle을 초래하면 edge를 버린다
		else {}; //do nothing
		
	}
	//(중요) 여기서 입력으로 주어진 것은 하나의 connected graph이라는 전제가 깔려 있다. 만약 connected이지 않으면 오류가 발생

	//결과를 출력한다
	printf("Following are the edges in the constructed MST\n");
	for (int i = 0; i < ans_cnt; i++) {
		printf("%d -- %d == %d\n", ans[i].from, ans[i].to, ans[i].weight);
	}
}
