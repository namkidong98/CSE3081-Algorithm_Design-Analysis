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
	int root;				//해당 group의 root
	int comp_cnt;			//해당 group에 속한 component의 개수
	long long weight_total;	//해당 group의 MST를 구성하는 weight의 총합
}Group;

int n_vertex;			//ASCII 파일에서 읽어온, vertex의 개수
int n_edge;				//ASCII 파일에서 읽어온, edge의 개수
int MAX_WEIGHT;			//ASCII 파일에서 읽어온, edge의 weight가 가장 가질 수 있는 가장 큰 값
edge_pointer Edge_Arr;	//Edge의 정보를 저장하는 배열
Group* group;			//connected graph 단위로 저장하는 배열
int group_cnt = 0;		//connected graph의 개수
int k_scanned = 0;		//MST를 생성할 때까지 읽은 edge의 개수
clock_t start, end;		//시간을 측정할 때 사용하는 변수

int find(subset subsets[], int i);				//해당 원소의 parent를 찾아서 반환하는 함수
void Union(subset subsets[], int x, int y);		//두 원소가 속한 집합을 병합(merge)하는 함수
void KruskalMST();								//disjoint set의 find, union을 바탕으로 구현한 MST를 구하는 Kruskal 알고리즘
void buildMinHeap(edge_pointer arr, int n);		//Minheap을 만드는 함수
void heapify(edge_pointer arr, int n, int i);	//heap의 성질을 유지하도록 하는 함수
edge extractMin(edge_pointer arr, int* n);		//제일 작은 root node를 min heap에서 꺼내는 함수

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
	strcpy(output, input); //output에도 저장

	//input file의 직접 주소 만들기
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

	fscanf(fp, "%d %d %d", &n_vertex, &n_edge, &MAX_WEIGHT); //파일의 첫 줄에서 vertex의 개수, edge의 개수, 최대 weight를 입력 받는다
	Edge_Arr = (edge_pointer)malloc(sizeof(edge) * n_edge);
	for (int i = 0; i < n_edge; i++) { //파일의 둘째 줄부터 edge의 개수만큼 edge의 정보를 입력 받는다
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
void KruskalMST() {
	int temp;
	group = (Group*)malloc(sizeof(Group) * n_vertex);
	subset* subsets = (subset*)malloc(n_vertex * sizeof(subset)); //Vertex 개수만큼 set의 공간을 할당(single node를 만들기 위해)
	for (int i = 0; i < n_vertex; i++) { //V subset을 single node로 만든다
		subsets[i].parent = i; //single node는 자기 스스로가 자신의 parent이다
		subsets[i].rank = 0; //rank는 root로부터 들어간 depth를 의미하므로 root node, single node의 rank는 0이다
	}

	//Union-Find로 connected graph를 만든다
	for (int i = 0; i < n_edge; i++) {
		edge next_edge = Edge_Arr[i];
		int x = find(subsets, next_edge.from); //next_edge의 출발 vertex를 포함한 집합의 root를 x로 받아옴
		int y = find(subsets, next_edge.to);   //next_edge의 도착 vertex를 포함한 집합의 root를 y로 받아옴
		if (x != y)
			Union(subsets, x, y);
	}

	//모든 vertex를 모두 검색하면서 connected graph가 몇 개 있는지, 해당 graph의 집합을 나타내는 root는 무엇인지 갱신
	for (int i = 0; i < n_vertex; i++) {
		if (i == find(subsets, i)) {
			group[group_cnt].root = i;			//해당 connected group의 root를 저장
			group[group_cnt].weight_total = 0;	//초기화
			group[group_cnt].comp_cnt = 0;		//초기화
			group_cnt++;						//connected group의 개수를 1 증가 
		}
	}

	//connected graph(group)의 component 개수를 계산하고 subset의 root를 계산하여 저장해놓는다
	for (int i = 0; i < n_vertex; i++) {
		temp = find(subsets, i);
		for (int j = 0; j < group_cnt; j++) {
			if (temp == group[j].root) { //connected graph(group)들 중 root가 같은 집합을 찾으면
				group[j].comp_cnt++;	 //해당 graph의 connected component의 수를 증가시킨다
				subsets[i].root = group[j].root;
				break;
			}
		}
	}

	int num_edge = n_edge;
	int in_edge = 0;
	buildMinHeap(Edge_Arr, n_edge);

	//subsets을 root빼고 초기화 시켜서 다시 find-union을 쓸 수 있는 상태로 만든다
	for (int i = 0; i < n_vertex; i++) {
		subsets[i].parent = i;
		subsets[i].rank = 0;
	}

	//모든 Edge를 검색하면서 
	while (num_edge > 0) {
		edge next_edge = extractMin(Edge_Arr, &num_edge);
		k_scanned++; //heap의 원소가 하나씩 빠질 때 마다 더해줘서 

		//edge의 출발 vertex와 도착 vertex의 집합이 같은지 find를 통해 검사한다
		int x = find(subsets, next_edge.from); //next_edge의 출발 vertex를 포함한 집합의 root를 x로 받아옴
		int y = find(subsets, next_edge.to);   //next_edge의 도착 vertex를 포함한 집합의 root를 y로 받아옴

		//만약 해당 edge(next_edge)가 cycle을 초래하지 않으면 포함한다
		//x != y : edge의 출발 vertex와 도착 vertex가 같은 집합이 아니었다는 뜻 --> edge를 포함해도 cycle이 발생하지 않는다
		//subsets[next_edge.from].root == subsets[next_edge.to].root : 둘이 같은 집합, 즉 같은 connected graph의 원소일 때
		if (x != y && subsets[x].root == subsets[y].root) {
			in_edge++;
			if (in_edge == n_vertex - 1)
				break;
			temp = subsets[x].root; //connected graph의 root를 temp에 저장하고
			for (int j = 0; j < group_cnt; j++) { //connected graph의 목록을 찾아보면서
				if (temp == group[j].root) { // 해당하는 connected graph을 찾으면
					group[j].weight_total += next_edge.weight;	 //해당 graph의 total weight를 증가시킨다
					break;
				}
			}
			Union(subsets, x, y);
		}
	}
	printf("k_scanned: %d   in edge: %d    n_vertex: %d\n", k_scanned, in_edge, n_vertex);
}
void buildMinHeap(edge_pointer arr, int n) { // 배열 기반으로 min heap 구성
	for (int i = n / 2 - 1; i >= 0; i--) { // 마지막 내부 노드부터 역순으로 heapify 수행
		heapify(arr, n, i);
	}
}
void heapify(edge_pointer arr, int n, int i) {
	int smallest = i;  // 현재 노드를 가장 작은 값으로 설정
	int left = 2 * i + 1;  // 왼쪽 자식 노드 인덱스
	int right = 2 * i + 2;  // 오른쪽 자식 노드 인덱스
	int tmp_weight, tmp_to, tmp_from;

	// 왼쪽 자식이 루트보다 작은 경우
	if (left < n && arr[left].weight < arr[smallest].weight) smallest = left;

	// 오른쪽 자식이 루트보다 작은 경우
	if (right < n && arr[right].weight < arr[smallest].weight) smallest = right;

	// 현재 루트와 가장 작은 값을 교환
	if (smallest != i) {
		tmp_weight = arr[i].weight; tmp_from = arr[i].from; tmp_to = arr[i].to;
		arr[i].weight = arr[smallest].weight; arr[i].from = arr[smallest].from; arr[i].to = arr[smallest].to;
		arr[smallest].weight = tmp_weight; arr[smallest].from = tmp_from; arr[smallest].to = tmp_to;

		// 변경된 자식 노드에 대해 재귀적으로 heapify 호출
		heapify(arr, n, smallest);
	}
}
edge extractMin(edge_pointer arr, int* n) { // min heap에서 최소값을 꺼내는 함수
	if (*n <= 0) {
		fprintf(stderr, "HEAP IS Empty!\n");
		exit(0);  // 힙이 비어있을 경우 -1 반환 또는 예외 처리
	}
	edge min_edge = arr[0];  // 최소값은 루트 노드에 위치
	arr[0] = arr[*n - 1];  // 힙의 마지막 노드를 루트로 이동
	(*n)--;  // 힙의 크기를 1 감소

	heapify(arr, *n, 0);  // 힙 속성 유지를 위해 heapify 호출

	return min_edge;
}
