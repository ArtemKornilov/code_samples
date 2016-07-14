#ifndef TDIGRAPH_H
#define TDIGRAPH_H

#include "vector.h"
#include "stack.h"

class TDigraph {
public:
	TDigraph(int size);
	virtual ~TDigraph();
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

TDigraph::~TDigraph() {
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

#endif