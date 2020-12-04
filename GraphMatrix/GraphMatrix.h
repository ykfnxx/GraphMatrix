#pragma once
#include "Vector.h" //引入向量
#include "Queue.h"
#include <iostream>

typedef enum { UNDISCOVERED, DISCOVERED, VISITED } VStatus; //顶点状态
typedef enum { UNDETERMINED, TREE, CROSS, FORWARD, BACKWARD } EType; //边在遍历树中所属的类型

template <typename Tv> struct Vertex { //顶点对象（为简化起见，并未严格封装）
	Tv data; int inDegree, outDegree; VStatus status; //数据、出入度数、状态
	int dTime, fTime; //时间标签
	int parent; int priority; //在遍历树中的父节点、优先级数
	Vertex(Tv const& d = (Tv)0) : //构造新顶点
		data(d), inDegree(0), outDegree(0), status(UNDISCOVERED),
		dTime(-1), fTime(-1), parent(-1), priority(INT_MAX) {} //暂不考虑权重溢出
};

template <typename Te> struct Edge { //边对象（为简化起见，并未严格封装）
	Te data; int weight; EType type; //数据、权重、类型
	Edge(Te const& d, int w) : data(d), weight(w), type(UNDETERMINED) {} //构造
};


template<typename Tv, typename Te>
class GraphMatrix {
private:
	Vector< Vertex< Tv > > V; //顶点集（向量）
	Vector< Vector< Edge< Te > * > > E; //边集（邻接矩阵）

	void BFS(int v, int& clock);//（连通域）广度优先搜索算法
	void DFS(int v, int& clock); //（连通域）深度优先搜索算法
	template <typename PU> void PFS(int, PU); //（连通域）优先级搜索框架


	void reset() { //所有顶点、边的辅助信息复位
		for (int i = 0; i < n; i++) { //所有顶点的
			status(i) = UNDISCOVERED; //状态
			parent(i) = -1; priority(i) = INT_MAX; //（在遍历树中的）父节点，优先级数
			for (int j = 0; j < n; j++) //所有边的
				if (exists(i, j)) type(i, j) = UNDETERMINED; //类型
		}
	}
public:
	int n; //顶点总数
	int e; //边总数

	void bfs(int); //广度优先搜索算法
	void dfs(int); //深度优先搜索算法
	void prim(int); //最小支撑树Prim算法
	void dijkstra(int); //最短路径Dijkstra算法
	template <typename PU> void pfs(int, PU); //优先级搜索框架

	GraphMatrix() { n = e = 0; } //构造
	~GraphMatrix() { //析构
		for (int j = 0; j < n; j++) //所有动态创建的
			for (int k = 0; k < n; k++) //边记录
				delete E[j][k]; //逐条清除
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

template <typename Tv, typename Te> //广度优先搜索BFS算法（全图）
void GraphMatrix<Tv, Te>::bfs(int s) { //assert: 0 <= s < n
	reset(); int clock = 0; int v = s; //初始化
	do //逐一检查所有顶点
		if (UNDISCOVERED == status(v)) //一旦遇到尚未发现的顶点
			BFS(v, clock); //即从该顶点出发启动一次BFS
	while (s != (v = (++v % n))); //按序号检查，故不漏不重
	printf("\n");
}

template <typename Tv, typename Te> //广度优先搜索BFS算法（单个连通域）
void GraphMatrix<Tv, Te>::BFS(int v, int& clock) { //assert: 0 <= v < n
	Queue<int> Q; //引入辅助队列
	status(v) = DISCOVERED; Q.enqueue(v); //初始化起点
	while (!Q.empty()) { //在Q变空之前，不断
		int v = Q.dequeue(); dTime(v) = ++clock; //取出队首顶点v
		for (int u = firstNbr(v); -1 < u; u = nextNbr(v, u)) //枚举v的所有邻居u
			if (UNDISCOVERED == status(u)) { //若u尚未被发现，则
				status(u) = DISCOVERED; Q.enqueue(u); //发现该顶点
				type(v, u) = TREE; parent(u) = v; //引入树边拓展支撑树
			}
			else { //若u已被发现，或者甚至已访问完毕，则
				type(v, u) = CROSS; //将(v, u)归类于跨边
			}
			status(v) = VISITED; //至此，当前顶点访问完毕
			printf("%c ", (char)vertex(v));
	}
}

template <typename Tv, typename Te> //深度优先搜索DFS算法（全图）
void GraphMatrix<Tv, Te>::dfs(int s) { //assert: 0 <= s < n
	reset(); int clock = 0; int v = s; //初始化
	do //逐一检查所有顶点
		if (UNDISCOVERED == status(v)) //一旦遇到尚未发现的顶点
			DFS(v, clock); //即从该顶点出发启动一次DFS
	while (s != (v = (++v % n))); //按序号检查，故不漏不重
	printf("\n");
}

template <typename Tv, typename Te> //深度优先搜索DFS算法（单个连通域）
void GraphMatrix<Tv, Te>::DFS(int v, int& clock) { //assert: 0 <= v < n
	dTime(v) = ++clock; status(v) = DISCOVERED; //发现当前顶点v
	printf("%c ", (char)vertex(v));
	for (int u = firstNbr(v); -1 < u; u = nextNbr(v, u)) //枚举v的所有邻居u
		switch (status(u)) { //并视其状态分别处理
		case UNDISCOVERED: //u尚未发现，意味着支撑树可在此拓展
			type(v, u) = TREE; parent(u) = v; DFS(u, clock); break;
		case DISCOVERED: //u已被发现但尚未访问完毕，应属被后代指向的祖先
			type(v, u) = BACKWARD;  break;
		default: //u已访问完毕（VISITED，有向图），则视承袭关系分为前向边或跨边
			type(v, u) = (dTime(v) < dTime(u)) ? FORWARD : CROSS; break;
		}
	status(v) = VISITED; fTime(v) = ++clock; //至此，当前顶点v方告访问完毕

}

template <typename Tv, typename Te> //最短路径Dijkstra算法：适用于一般的有向图
void GraphMatrix<Tv, Te>::dijkstra(int s) { //assert: 0 <= s < n
	reset(); priority(s) = 0;
	for (int i = 0; i < n; i++) { //共需引入n个顶点和n-1条边
		status(s) = VISITED;
		if (parent(s) == -1)
			printf("(%c) ", (char)vertex(s));
		else
			printf("(%c,%c) ", (char)vertex(parent(s)), (char)vertex(s));
		if (-1 != parent(s)) type(parent(s), s) = TREE; //引入当前的s
		for (int j = firstNbr(s); -1 < j; j = nextNbr(s, j)) //枚举s的所有邻居j
			if ((status(j) == UNDISCOVERED) && (priority(j) > priority(s) + weight(s, j))) //对邻接顶点j做松弛
			{
				priority(j) = priority(s) + weight(s, j); parent(j) = s;
			} //与Prim算法唯一的不同之处
		for (int shortest = INT_MAX, j = 0; j < n; j++) //选出下一最近顶点
			if ((status(j) == UNDISCOVERED) && (shortest > priority(j)))
			{
				shortest = priority(j); s = j;
			}
	}
	printf("\n");
} //对于无向连通图，假设每一条边表示为方向互逆、权重相等的一对边

template <typename Tv, typename Te> //Prim算法：无向连通图，各边表示为方向互逆、权重相等的一对边
void GraphMatrix<Tv, Te>::prim(int s) { //assert: 0 <= s < n
	reset(); priority(s) = 0;
	for (int i = 0; i < n; i++) { //共需引入n个顶点和n-1条边
		status(s) = VISITED;
		if (parent(s) == -1)
			printf("(%c) ", (char)vertex(s));
		else
			printf("(%c,%c) ", (char)vertex(parent(s)), (char)vertex(s));
		if (-1 != parent(s)) type(parent(s), s) = TREE; //引入当前的s
		for (int j = firstNbr(s); -1 < j; j = nextNbr(s, j)) //枚举s的所有邻居j
			if ((status(j) == UNDISCOVERED) && (priority(j) > weight(s, j))) //对邻接顶点j做松弛
			{
				priority(j) = weight(s, j); parent(j) = s;
			} //与Dijkstra算法唯一的不同之处
		for (int shortest = INT_MAX, j = 0; j < n; j++) //选出下一极短跨边
			if ((status(j) == UNDISCOVERED) && (shortest > priority(j)))
			{
				shortest = priority(j); s = j;
			}
	}
	printf("\n");
}



template <typename Tv, typename Te> template <typename PU> //优先级搜索（全图）
void GraphMatrix<Tv, Te>::pfs(int s, PU prioUpdater) { //assert: 0 <= s < n
	reset(); int v = s; //初始化
	do //逐一检查所有顶点
		if (UNDISCOVERED == status(v)) //一旦遇到尚未发现的顶点
			PFS(v, prioUpdater); //即从该顶点出发启动一次PFS
	while (s != (v = (++v % n))); //按序号检查，故不漏不重
}

template <typename Tv, typename Te> template <typename PU> //顶点类型、边类型、优先级更新器
void GraphMatrix<Tv, Te>::PFS(int s, PU prioUpdater) { //优先级搜索（单个连通域）
	priority(s) = 0; status(s) = VISITED; parent(s) = -1; //初始化，起点s加至PFS树中
	printf("(%c) ", (char)vertex(s));
	while (1) { //将下一顶点和边加至PFS树中
		for (int w = firstNbr(s); -1 < w; w = nextNbr(s, w)) //枚举s的所有邻居w
			prioUpdater(this, s, w); //更新顶点w的优先级及其父顶点
		for (int shortest = INT_MAX, w = 0; w < n; w++)
			if (UNDISCOVERED == status(w)) //从尚未加入遍历树的顶点中
				if (shortest > priority(w)) //选出下一个
				{
					shortest = priority(w); s = w;
				} //优先级最高的顶点s
		if (VISITED == status(s)) break; //直至所有顶点均已加入
		status(s) = VISITED; type(parent(s), s) = TREE; //将s及与其父的联边加入遍历树

		printf("(%c,%c) ", (char)vertex(parent(s)), (char)vertex(s));

	}
	printf("\n");
} //通过定义具体的优先级更新策略prioUpdater，即可实现不同的算法功能

template <typename Tv, typename Te> struct BfsPU { //针对BFS算法的顶点优先级更新器
	virtual void operator() (GraphMatrix<Tv, Te>* g, int uk, int v) {
		if (g->status(v) == UNDISCOVERED) //对于uk每一尚未被发现的邻接顶点v
			if (g->priority(v) > g->priority(uk) + 1) { //将其到起点的距离作为优先级数
				g->priority(v) = g->priority(uk) + 1; //更新优先级（数）
				g->parent(v) = uk; //更新父节点
			} //如此效果等同于，先被发现者优先
	}
};

template <typename Tv, typename Te> struct DfsPU { //针对DFS算法的顶点优先级更新器
	virtual void operator() (GraphMatrix<Tv, Te>* g, int uk, int v) {
		if (g->status(v) == UNDISCOVERED) //对于uk每一尚未被发现的邻接顶点v
			if (g->priority(v) > g->priority(uk) - 1) { //将其到起点距离的负数作为优先级数
				g->priority(v) = g->priority(uk) - 1; //更新优先级（数）
				g->parent(v) = uk; //更新父节点
				return; //注意：与BfsPU()不同，这里只要有一个邻接顶点可更新，即可立即返回
			} //如此效果等同于，后被发现者优先
	}
};

template <typename Tv, typename Te> struct DijkstraPU { //针对Dijkstra算法的顶点优先级更新器
	virtual void operator() (GraphMatrix<Tv, Te>* g, int uk, int v) {
		if (UNDISCOVERED == g->status(v)) //对于uk每一尚未被发现的邻接顶点v，按Dijkstra策略
			if (g->priority(v) > g->priority(uk) + g->weight(uk, v)) { //做松弛
				g->priority(v) = g->priority(uk) + g->weight(uk, v); //更新优先级（数）
				g->parent(v) = uk; //并同时更新父节点
			}
	}
};

template <typename Tv, typename Te> struct PrimPU { //针对Prim算法的顶点优先级更新器
	virtual void operator() (GraphMatrix<Tv, Te>* g, int uk, int v) {
		if (UNDISCOVERED == g->status(v)) //对于uk每一尚未被发现的邻接顶点v
			if (g->priority(v) > g->weight(uk, v)) { //按Prim策略做松弛
				g->priority(v) = g->weight(uk, v); //更新优先级（数）
				g->parent(v) = uk; //更新父节点
			}
	}
};

