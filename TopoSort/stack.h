#ifndef TSTACK_H
#define TSTACK_H


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
	Head = nullptr;
}

template <class T>
TStack<T>::~TStack() {

	TNode *current = Head;
	TNode *tmp;
	while (current != nullptr) {
		tmp = current->Next;
		delete current;
		current = tmp;
	}

}

template <class T>
bool TStack<T>::IsEmpty() {

	return Head == nullptr;
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

#endif