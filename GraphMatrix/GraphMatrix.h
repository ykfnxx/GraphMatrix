#pragma once
#include "Vector.h" //��������
#include "Queue.h"
#include <iostream>

typedef enum { UNDISCOVERED, DISCOVERED, VISITED } VStatus; //����״̬
typedef enum { UNDETERMINED, TREE, CROSS, FORWARD, BACKWARD } EType; //���ڱ�����������������

template <typename Tv> struct Vertex { //�������Ϊ���������δ�ϸ��װ��
	Tv data; int inDegree, outDegree; VStatus status; //���ݡ����������״̬
	int dTime, fTime; //ʱ���ǩ
	int parent; int priority; //�ڱ������еĸ��ڵ㡢���ȼ���
	Vertex(Tv const& d = (Tv)0) : //�����¶���
		data(d), inDegree(0), outDegree(0), status(UNDISCOVERED),
		dTime(-1), fTime(-1), parent(-1), priority(INT_MAX) {} //�ݲ�����Ȩ�����
};

template <typename Te> struct Edge { //�߶���Ϊ���������δ�ϸ��װ��
	Te data; int weight; EType type; //���ݡ�Ȩ�ء�����
	Edge(Te const& d, int w) : data(d), weight(w), type(UNDETERMINED) {} //����
};


template<typename Tv, typename Te>
class GraphMatrix {
private:
	Vector< Vertex< Tv > > V; //���㼯��������
	Vector< Vector< Edge< Te > * > > E; //�߼����ڽӾ���

	void BFS(int v, int& clock);//����ͨ�򣩹�����������㷨
	void DFS(int v, int& clock); //����ͨ��������������㷨
	template <typename PU> void PFS(int, PU); //����ͨ�����ȼ��������


	void reset() { //���ж��㡢�ߵĸ�����Ϣ��λ
		for (int i = 0; i < n; i++) { //���ж����
			status(i) = UNDISCOVERED; //״̬
			parent(i) = -1; priority(i) = INT_MAX; //���ڱ������еģ����ڵ㣬���ȼ���
			for (int j = 0; j < n; j++) //���бߵ�
				if (exists(i, j)) type(i, j) = UNDETERMINED; //����
		}
	}
public:
	int n; //��������
	int e; //������

	void bfs(int); //������������㷨
	void dfs(int); //������������㷨
	void prim(int); //��С֧����Prim�㷨
	void dijkstra(int); //���·��Dijkstra�㷨
	template <typename PU> void pfs(int, PU); //���ȼ��������

	GraphMatrix() { n = e = 0; } //����
	~GraphMatrix() { //����
		for (int j = 0; j < n; j++) //���ж�̬������
			for (int k = 0; k < n; k++) //�߼�¼
				delete E[j][k]; //�������
	}
	Tv& vertex(int i) { return V[i].data; };
	int inDegree(int i) { return V[i].inDegree; };
	int outDegree(int i) { return V[i].outDegree; };
	int firstNbr(int i) { return nextNbr(i, n); };
	int nextNbr(int i, int j) {
		while ((-1 < j) && (!exists(i, --j)));
		return j;
	}
	VStatus& status(int i) { return V[i].status; };
	int& dTime(int i) { return V[i].dTime; };
	int& fTime(int i) { return V[i].fTime; };
	int& parent(int i) { return V[i].parent; };
	int& priority(int i) { return V[i].priority; };

	int insert(Tv const& vertex) {
		for (int j = 0; j < n; j++)
			E[j].insert(NULL);
		n++;
		E.insert(Vector<Edge<Te>*>(n, n, (Edge<Te>*)NULL));
		return V.insert(Vertex<Tv>(vertex));
	}

	Tv remove(int i) {
		for (int j = 0; j < n; j++)
			if (exists(i, j)) { delete E[i][j]; V[j].inDegree--; }
		E.remove(i);
		n--;
		Tv vBak = vertex(i); V.remove(i);
		for (int j = 0; j < n; j++)
			if (Edge<Te>* e = E[j].remove(i)) {
				delete e;
				V[j].outDegree--;
			}
		return vBak;
	}

	bool exists(int i, int j) {
		return (0 <= i) && (i < n) && (0 <= j) && (j < n) && E[i][j] != NULL;
	}

	EType& type(int i, int j) { return E[i][j]->type; };
	Te& edge(int i, int j) { return E[i][j]->data; };
	int& weight(int i, int j) { return E[i][j]->weight; };

	void insert(Te const& edge, int w, int i, int j) {
		if (exists(i, j)) return;
		E[i][j] = new Edge<Te>(edge, w);
		e++;
		V[i].outDegree++;
		V[j].inDegree++;
	}

	Te remove(int i, int j) {
		Te eBak = edge(i, j);
		delete E[i][j];
		E[i][j] = NULL;
		e--;
		V[i].outDegree--;
		V[j].inDegree--;
		return eBak;
	}
};

template <typename Tv, typename Te> //�����������BFS�㷨��ȫͼ��
void GraphMatrix<Tv, Te>::bfs(int s) { //assert: 0 <= s < n
	reset(); int clock = 0; int v = s; //��ʼ��
	do //��һ������ж���
		if (UNDISCOVERED == status(v)) //һ��������δ���ֵĶ���
			BFS(v, clock); //���Ӹö����������һ��BFS
	while (s != (v = (++v % n))); //����ż�飬�ʲ�©����
	printf("\n");
}

template <typename Tv, typename Te> //�����������BFS�㷨��������ͨ��
void GraphMatrix<Tv, Te>::BFS(int v, int& clock) { //assert: 0 <= v < n
	Queue<int> Q; //���븨������
	status(v) = DISCOVERED; Q.enqueue(v); //��ʼ�����
	while (!Q.empty()) { //��Q���֮ǰ������
		int v = Q.dequeue(); dTime(v) = ++clock; //ȡ�����׶���v
		for (int u = firstNbr(v); -1 < u; u = nextNbr(v, u)) //ö��v�������ھ�u
			if (UNDISCOVERED == status(u)) { //��u��δ�����֣���
				status(u) = DISCOVERED; Q.enqueue(u); //���ָö���
				type(v, u) = TREE; parent(u) = v; //����������չ֧����
			}
			else { //��u�ѱ����֣����������ѷ�����ϣ���
				type(v, u) = CROSS; //��(v, u)�����ڿ��
			}
			status(v) = VISITED; //���ˣ���ǰ����������
			printf("%c ", (char)vertex(v));
	}
}

template <typename Tv, typename Te> //�����������DFS�㷨��ȫͼ��
void GraphMatrix<Tv, Te>::dfs(int s) { //assert: 0 <= s < n
	reset(); int clock = 0; int v = s; //��ʼ��
	do //��һ������ж���
		if (UNDISCOVERED == status(v)) //һ��������δ���ֵĶ���
			DFS(v, clock); //���Ӹö����������һ��DFS
	while (s != (v = (++v % n))); //����ż�飬�ʲ�©����
	printf("\n");
}

template <typename Tv, typename Te> //�����������DFS�㷨��������ͨ��
void GraphMatrix<Tv, Te>::DFS(int v, int& clock) { //assert: 0 <= v < n
	dTime(v) = ++clock; status(v) = DISCOVERED; //���ֵ�ǰ����v
	printf("%c ", (char)vertex(v));
	for (int u = firstNbr(v); -1 < u; u = nextNbr(v, u)) //ö��v�������ھ�u
		switch (status(u)) { //������״̬�ֱ���
		case UNDISCOVERED: //u��δ���֣���ζ��֧�������ڴ���չ
			type(v, u) = TREE; parent(u) = v; DFS(u, clock); break;
		case DISCOVERED: //u�ѱ����ֵ���δ������ϣ�Ӧ�������ָ�������
			type(v, u) = BACKWARD;  break;
		default: //u�ѷ�����ϣ�VISITED������ͼ�������ӳ�Ϯ��ϵ��Ϊǰ��߻���
			type(v, u) = (dTime(v) < dTime(u)) ? FORWARD : CROSS; break;
		}
	status(v) = VISITED; fTime(v) = ++clock; //���ˣ���ǰ����v����������

}

template <typename Tv, typename Te> //���·��Dijkstra�㷨��������һ�������ͼ
void GraphMatrix<Tv, Te>::dijkstra(int s) { //assert: 0 <= s < n
	reset(); priority(s) = 0;
	for (int i = 0; i < n; i++) { //��������n�������n-1����
		status(s) = VISITED;
		if (parent(s) == -1)
			printf("(%c) ", (char)vertex(s));
		else
			printf("(%c,%c) ", (char)vertex(parent(s)), (char)vertex(s));
		if (-1 != parent(s)) type(parent(s), s) = TREE; //���뵱ǰ��s
		for (int j = firstNbr(s); -1 < j; j = nextNbr(s, j)) //ö��s�������ھ�j
			if ((status(j) == UNDISCOVERED) && (priority(j) > priority(s) + weight(s, j))) //���ڽӶ���j���ɳ�
			{
				priority(j) = priority(s) + weight(s, j); parent(j) = s;
			} //��Prim�㷨Ψһ�Ĳ�֮ͬ��
		for (int shortest = INT_MAX, j = 0; j < n; j++) //ѡ����һ�������
			if ((status(j) == UNDISCOVERED) && (shortest > priority(j)))
			{
				shortest = priority(j); s = j;
			}
	}
	printf("\n");
} //����������ͨͼ������ÿһ���߱�ʾΪ�����桢Ȩ����ȵ�һ�Ա�

template <typename Tv, typename Te> //Prim�㷨��������ͨͼ�����߱�ʾΪ�����桢Ȩ����ȵ�һ�Ա�
void GraphMatrix<Tv, Te>::prim(int s) { //assert: 0 <= s < n
	reset(); priority(s) = 0;
	for (int i = 0; i < n; i++) { //��������n�������n-1����
		status(s) = VISITED;
		if (parent(s) == -1)
			printf("(%c) ", (char)vertex(s));
		else
			printf("(%c,%c) ", (char)vertex(parent(s)), (char)vertex(s));
		if (-1 != parent(s)) type(parent(s), s) = TREE; //���뵱ǰ��s
		for (int j = firstNbr(s); -1 < j; j = nextNbr(s, j)) //ö��s�������ھ�j
			if ((status(j) == UNDISCOVERED) && (priority(j) > weight(s, j))) //���ڽӶ���j���ɳ�
			{
				priority(j) = weight(s, j); parent(j) = s;
			} //��Dijkstra�㷨Ψһ�Ĳ�֮ͬ��
		for (int shortest = INT_MAX, j = 0; j < n; j++) //ѡ����һ���̿��
			if ((status(j) == UNDISCOVERED) && (shortest > priority(j)))
			{
				shortest = priority(j); s = j;
			}
	}
	printf("\n");
}



template <typename Tv, typename Te> template <typename PU> //���ȼ�������ȫͼ��
void GraphMatrix<Tv, Te>::pfs(int s, PU prioUpdater) { //assert: 0 <= s < n
	reset(); int v = s; //��ʼ��
	do //��һ������ж���
		if (UNDISCOVERED == status(v)) //һ��������δ���ֵĶ���
			PFS(v, prioUpdater); //���Ӹö����������һ��PFS
	while (s != (v = (++v % n))); //����ż�飬�ʲ�©����
}

template <typename Tv, typename Te> template <typename PU> //�������͡������͡����ȼ�������
void GraphMatrix<Tv, Te>::PFS(int s, PU prioUpdater) { //���ȼ�������������ͨ��
	priority(s) = 0; status(s) = VISITED; parent(s) = -1; //��ʼ�������s����PFS����
	printf("(%c) ", (char)vertex(s));
	while (1) { //����һ����ͱ߼���PFS����
		for (int w = firstNbr(s); -1 < w; w = nextNbr(s, w)) //ö��s�������ھ�w
			prioUpdater(this, s, w); //���¶���w�����ȼ����丸����
		for (int shortest = INT_MAX, w = 0; w < n; w++)
			if (UNDISCOVERED == status(w)) //����δ����������Ķ�����
				if (shortest > priority(w)) //ѡ����һ��
				{
					shortest = priority(w); s = w;
				} //���ȼ���ߵĶ���s
		if (VISITED == status(s)) break; //ֱ�����ж�����Ѽ���
		status(s) = VISITED; type(parent(s), s) = TREE; //��s�����丸�����߼��������

		printf("(%c,%c) ", (char)vertex(parent(s)), (char)vertex(s));

	}
	printf("\n");
} //ͨ�������������ȼ����²���prioUpdater������ʵ�ֲ�ͬ���㷨����

template <typename Tv, typename Te> struct BfsPU { //���BFS�㷨�Ķ������ȼ�������
	virtual void operator() (GraphMatrix<Tv, Te>* g, int uk, int v) {
		if (g->status(v) == UNDISCOVERED) //����ukÿһ��δ�����ֵ��ڽӶ���v
			if (g->priority(v) > g->priority(uk) + 1) { //���䵽���ľ�����Ϊ���ȼ���
				g->priority(v) = g->priority(uk) + 1; //�������ȼ�������
				g->parent(v) = uk; //���¸��ڵ�
			} //���Ч����ͬ�ڣ��ȱ�����������
	}
};

template <typename Tv, typename Te> struct DfsPU { //���DFS�㷨�Ķ������ȼ�������
	virtual void operator() (GraphMatrix<Tv, Te>* g, int uk, int v) {
		if (g->status(v) == UNDISCOVERED) //����ukÿһ��δ�����ֵ��ڽӶ���v
			if (g->priority(v) > g->priority(uk) - 1) { //���䵽������ĸ�����Ϊ���ȼ���
				g->priority(v) = g->priority(uk) - 1; //�������ȼ�������
				g->parent(v) = uk; //���¸��ڵ�
				return; //ע�⣺��BfsPU()��ͬ������ֻҪ��һ���ڽӶ���ɸ��£�������������
			} //���Ч����ͬ�ڣ��󱻷���������
	}
};

template <typename Tv, typename Te> struct DijkstraPU { //���Dijkstra�㷨�Ķ������ȼ�������
	virtual void operator() (GraphMatrix<Tv, Te>* g, int uk, int v) {
		if (UNDISCOVERED == g->status(v)) //����ukÿһ��δ�����ֵ��ڽӶ���v����Dijkstra����
			if (g->priority(v) > g->priority(uk) + g->weight(uk, v)) { //���ɳ�
				g->priority(v) = g->priority(uk) + g->weight(uk, v); //�������ȼ�������
				g->parent(v) = uk; //��ͬʱ���¸��ڵ�
			}
	}
};

template <typename Tv, typename Te> struct PrimPU { //���Prim�㷨�Ķ������ȼ�������
	virtual void operator() (GraphMatrix<Tv, Te>* g, int uk, int v) {
		if (UNDISCOVERED == g->status(v)) //����ukÿһ��δ�����ֵ��ڽӶ���v
			if (g->priority(v) > g->weight(uk, v)) { //��Prim�������ɳ�
				g->priority(v) = g->weight(uk, v); //�������ȼ�������
				g->parent(v) = uk; //���¸��ڵ�
			}
	}
};

