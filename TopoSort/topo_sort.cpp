#include <iostream>

static const int REALLOC_MULTIPLIER = 2;

template <class T>
class TVector {
public:
	TVector();
	TVector(int &size);
	virtual ~TVector();

	void Resize(int size);
	int Size();
	T &operator[](int i);
	void Append(T data);
	void Realloc();

private:
	T *Vector;
	T Junk;
	int MaxSize;
	int Current;
};

template <class T>
TVector<T>::TVector() {

	Vector = NULL;
	MaxSize = 0;
	Current = 0;
}

template <class T>
TVector<T>::TVector(int &size) {

	Vector = new T[size];
	MaxSize = size;
	Current = size;
}

template <class T>
TVector<T>::~TVector() {

	if (Current) {
		delete[] Vector;
	}
}

template <class T>
T &TVector<T>::operator[](int i) {

	if (i < Current && i >= 0) {
		return *(Vector + i);
	}
	else {
		return Junk;
	}
}


template <class T>
void TVector<T>::Append(T data) {

	if (Current == MaxSize) {
		TVector::Realloc();
	}

	Vector[Current] = data;
	Current++;
}

template <class T>
void TVector<T>::Resize(int size) {

	T *tmp = new T[size];

	if (Current) {
		for (int i = 0; i < Current && i < size; ++i) {
			tmp[i] = Vector[i];
		}

		delete[] Vector;
	}

	Vector = tmp;
	Current = size;
	MaxSize = size;
}


template <class T>
int TVector<T>::Size() {

	return Current;
}

template <class T>
void TVector<T>::Realloc() {

	if (Current == 0) {
		MaxSize = 1;
		Vector = new T[MaxSize];
	}
	else {
		MaxSize *= REALLOC_MULTIPLIER;
		T *tmp = new T[MaxSize];
		for (int i = 0; i < Current; ++i) {
			tmp[i] = Vector[i];
		}

		delete[] Vector;
		Vector = tmp;
	}
}


template <class T>
class TStack {
public:
	TStack();
	virtual ~TStack();
	bool IsEmpty();
	bool Contain(T data);
	void Push(T data);
	T Pop();

private:
	struct TNode {
		TNode *Next;
		T Data;
	};
	TNode *Head;
	T Junk;
};

template <class T>
TStack<T>::TStack() {
	Head = NULL;
}

template <class T>
TStack<T>::~TStack() {

	TNode *current = Head;
	TNode *tmp;
	while (current != NULL) {
		tmp = current->Next;
		delete current;
		current = tmp;
	}

}

template <class T>
bool TStack<T>::IsEmpty() {

	return Head == NULL;
}

template <class T>
bool TStack<T>::Contain(T data) {

	TNode *current = Head;
	while (current) {
		if (current->Data == data) {
			return true;
		}
		current = current->Next;
	}
	
	return false;
}

template <class T>
void TStack<T>::Push(T data) {

	TNode *node = new TNode();
	node->Data = data; 
	node->Next = Head;
	Head = node;
}

template <class T>
T TStack<T>::Pop() {

	if (IsEmpty()) {
		return Junk;
	}

	T Data = Head->Data;
	TNode *oldHead = Head;
	Head = Head->Next;

	delete oldHead;
	return Data;
}


class TDigraph {
public:
	TDigraph(int size);
	virtual ~TDigraph(){};
	void AddConnection(int a, int  b);
	int GetClearVertice();
	bool IsDigraph();

private:
	void FindClearVertices();

	TVector< TStack<int> > OutcomeVertices;
	TVector<int> IncomeVerticesCounter;
	TStack<int> ClearVertices;
	int Size;
};

TDigraph::TDigraph(int size) {

	Size = size;
	OutcomeVertices.Resize(size);
	IncomeVerticesCounter.Resize(size);

	for (int i = 0; i < size; i++) {
		IncomeVerticesCounter[i] = 0;
	}
}

bool TDigraph::IsDigraph() {

	FindClearVertices();
	return !ClearVertices.IsEmpty();
}

void TDigraph::FindClearVertices() {

	for (int i = 0; i < Size; i++) {
		if (!IncomeVerticesCounter[i]) {
			ClearVertices.Push(i);
		}
	} 
}

void TDigraph::AddConnection(int a, int b) {

	if (!OutcomeVertices[a - 1].Contain(b - 1)) {
		OutcomeVertices[a - 1].Push(b - 1);
		IncomeVerticesCounter[b - 1]++;
	}
}

int TDigraph::GetClearVertice() {

	if (ClearVertices.IsEmpty()) {
		FindClearVertices();
	}

	int current;
	int vertice = ClearVertices.Pop();
	while (!OutcomeVertices[vertice].IsEmpty()) {

		current = OutcomeVertices[vertice].Pop();
		IncomeVerticesCounter[current]--;
		if (!IncomeVerticesCounter[current]) {
			ClearVertices.Push(current);
		}
	}

	return vertice + 1;
}


int main() {

	int n, m; 
	std::cin >> n >> m;
 	

	TDigraph graph(n);

 	int a, b;
	for (int i = 1; i <= m; i++) {
		std::cin >> a >> b;
		graph.AddConnection(a, b);
	}

	if (graph.IsDigraph()) {
		for (int i = 1; i <= n; i++) {
			std::cout << graph.GetClearVertice() << ' ';
		}	
	}
	else {
		std::cout << -1;
	}
	std::cout << std::endl;

	return 0;
}