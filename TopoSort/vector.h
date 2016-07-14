#ifndef VECTOR_H
#define VECTOR_H


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

	Vector = nullptr;
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

#endif