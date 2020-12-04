// GraphMatrix.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "GraphMatrix.h"

/******************************************************************************************
* �������У��ļ��ض����е���ͼ
******************************************************************************************/
void importGraph(GraphMatrix<char, int> & g, char fname[]) {
	FILE* input = fopen(fname, "r");
	int n; fscanf(input, "%d\n", &n); /*DSA*/printf("%d vertices\n", n);
	for (int i = 0; i < n; i++) { //����v������
		char vertex; fscanf(input, "%c ", &vertex);
		g.insert(vertex);
	}
	fscanf(input, "\n");
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) { //�����
			int edge; fscanf(input, "%d ", &edge);
			if (0 > edge) continue;
			g.insert(edge, edge, i, j);
		}
		fscanf(input, "\n");
	}
}

int main()
{
	GraphMatrix<char, int> g1;
	importGraph(g1, "G1.txt");
	printf("=== BFS\n");
	g1.bfs(0);

	printf("=== BFS (PFS)\n");
	g1.pfs(0, BfsPU<char, int>());

	printf("=== DFS\n");
	g1.dfs(0);

	printf("=== DFS (PFS)\n");
	g1.pfs(0, DfsPU<char, int>());


	printf("\n\n");
	GraphMatrix<char, int> g2;
	importGraph(g2, "G2.txt");
	printf("=== Prim\n");
	g2.prim(0);

	printf("=== DFS (PrimPU)\n");
	g2.pfs(0, PrimPU<char, int>());

	printf("=== Dijkstra\n");
	g2.dijkstra(0);

	printf("=== DFS (DijkstraPU)\n");
	g2.pfs(0, DijkstraPU<char, int>());
    return 0;
}

