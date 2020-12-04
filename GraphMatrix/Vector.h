#pragma once
#define DEFAULT_CAPACITY 3
typedef int Rank;

template <typename T> class Vector {

protected:
	int _capacity;
	int _size;
	T* _elem;

	void copyFrom(T const* A, Rank lo, Rank hi);
	void expand();
	void shrink();
	void merge(Rank lo, Rank mi, Rank hi);

public:
	Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0) {
		_capacity = c;
		_size = s;
		_elem = new T[_capacity];
		for (int i = 0; i < _size; i++) {
			_elem[i] = v;
		}
	};

	Vector(T const* A, Rank lo, Rank hi) { copyFrom(A, lo, hi); };
	Vector(T const* A, Rank n) { Vector(A, 0, n); };
	Vector(Vector<T> const& V, Rank lo, Rank hi) { copyFrom(V._elem, lo, hi); };
	Vector(Vector<T> const& V, Rank n) {Vector(V,0,n)};

	~Vector() { delete[] _elem; };

	T& operator [] (Rank n) { return _elem[n]; };
	Vector<T>& operator = (Vector<T> const& V);

	Rank insert(Rank n, T const& e);
	Rank insert(T const& e) { return insert(_size, e); };

	T remove(Rank n) { T e = _elem[n]; remove(n, n + 1); return e; };
	int remove(Rank lo, Rank hi);

	void bubbleSort(Rank lo, Rank hi);
	void bubbleSort() { bubbleSort(0, _size); };

	void selectSort(Rank lo, Rank hi);
	void selectSort() { selectSort(0, _size); };

	void insertSort(Rank lo, Rank hi);
	void insertSort() { insertSort(0, _size); };

	void mergeSort(Rank lo, Rank hi);
	void mergeSort() { mergeSort(0, _size); };

	template <typename VST> void traverse(VST& visit);
};

template <typename T>
void Vector<T>::bubbleSort(Rank lo, Rank hi) {
	int times = 0; bool exchange = true;             // 从第一趟开始
	int nSort = hi - lo;
	while (times < nSort && exchange) {
		exchange = false;                    // 某趟是否有交换的标志，初始为无交换
		for (int j = hi-1; j > lo+times; j--)    //从最后元素开始到第一个未排序元素
			if (_elem[j - 1]>_elem[j]) {              //若需要交换则置换元素          
				T temp = _elem[j - 1];
				_elem[j - 1] = _elem[j];
				_elem[j] = temp;
				exchange = true;
			}
		times++;
	}
}


template <typename T>
void Vector<T>::insertSort(Rank lo, Rank hi) { //assert: 0 < lo <= hi <= size
	for (int j = lo + 1; j < hi; j++) { // 循环新插入的元素
		T key = _elem[j]; // 缓存新插入元素 
		int i = j - 1;       // 已排序序列最后一个元素
		while (i >= 0 && _elem[i] > key) { // 对已排序序列从后往前比较
			_elem[i + 1] = _elem[i];   // 大于新插入元素则往后移
			i--;
		}
		_elem[i + 1] = key;  //插入新元素
	}
}



template <typename T>
void Vector<T>::selectSort(Rank lo, Rank hi) {
	for (Rank i = hi - 1; i > lo; i--) { // 从后往前
		int max = i;
		for (Rank j = 0; j < i + 1; j++) { // 遍历前面未排序，选择最大元素
			if (_elem[j] > _elem[max]) max = j;
		}
		if (max != i) {              // 交换
			T temp = _elem[i];
			_elem[i] = _elem[max];
			_elem[max] = temp;
		}
	}
}

template <typename T> //向量归并排序
void Vector<T>::mergeSort(Rank lo, Rank hi) { //0 <= lo < hi <= size
	if (hi - lo < 2) return; //单元素区间自然有序，否则...
	int mi = (lo + hi) / 2; //以中点为界
	mergeSort(lo, mi); mergeSort(mi, hi); //分别排序
	merge(lo, mi, hi); //归并
}

template <typename T> //有序向量的归并
void Vector<T>::merge(Rank lo, Rank mi, Rank hi) { //各自有序的子向量[lo, mi)和[mi, hi)
	T* A = _elem + lo; //合并后的向量A[0, hi - lo) = _elem[lo, hi)
	int lb = mi - lo; T* B = new T[lb]; //前子向量B[0, lb) = _elem[lo, mi)
	for (Rank i = 0; i < lb; B[i] = A[i++]); //复制前子向量
	int lc = hi - mi; T* C = _elem + mi; //后子向量C[0, lc) = _elem[mi, hi)
	for (Rank i = 0, j = 0, k = 0; (j<lb) || (k <lc);) { //B[j]和C[k]中的小者续至A末尾
		if ((j < lb) && (!(k < lc) || (B[j] <= C[k]))) A[i++] = B[j++];
		if ((k < lc) && (!(j < lb) || (C[k] <  B[j]))) A[i++] = C[k++];
	}
	delete[] B; //释放临时空间B
} //归并后得到完整的有序向量[lo, hi)


template <typename T>
int Vector<T>::remove(Rank lo, Rank hi) {
	int inter = hi - lo;
	for (int i = hi; i < _size; i++) {
		_elem[i - inter] = _elem[i];
	}
	_size = _size - inter;
	shrink();
	return inter;
}

template <typename T>
void Vector<T>::expand() {
	if (_size < _capacity) return;
	if (_capacity < DEFAULT_CAPACITY) _capacity = DEFAULT_CAPACITY;
	_capacity = 2 * _capacity;
	T* oldElem = _elem;
	_elem = new T[_capacity];
	for (int i = 0; i < _size; i++) {
		_elem[i] = oldElem[i];
	}
	delete[] oldElem;
}

template <typename T>
void Vector<T>::shrink() {
	if (_capacity < 2 * DEFAULT_CAPACITY) return;
	if (_size * 4 > _capacity) return;
	T* oldElem = _elem;
	_capacity = _capacity >> 1;
	_elem = new T[_capacity];
	for (int i = 0; i < _size; i++) {
		_elem[i] = oldElem[i];
	}
	delete[] oldElem;

}






template <typename T>
Rank Vector<T>::insert(Rank n, T const& e) {
	expand();
	for (int i = _size; i > n; i--) {
		_elem[i] = _elem[i - 1];
	}
	_elem[n] = e;
	_size++;
	return n;
}

template <typename T> template <typename VST> 
void Vector<T>::traverse(VST& visit) {
	for (int i = 0; i < _size; i++) {
		visit(_elem[i]);
	}
}

template <typename T>
Vector<T>& Vector<T>::operator = (Vector<T> const& V) {
	if (_elem) delete[] _elem;
	copyFrom(V._elem, 0, V._size);
	return *this;
}

template <typename T>
void Vector<T>::copyFrom(T const* A, Rank lo, Rank hi) {
	_capacity = 2*(hi-lo);
	_size = hi-lo;
	_elem = new T[_capacity];
	for (int i = 0; i < _size; i++) {
		_elem[i] = A[lo+i];
	}
}