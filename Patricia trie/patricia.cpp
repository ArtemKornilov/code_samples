#include <iostream>
#include <ctime>
#include <stdio.h>

static const int MAX_KEY_LENGTH = 257;
static const int LEFT_CHILD = 0;
static const int RIGHT_CHILD = 1;
static const int NUMBER_OF_CHILDREN = 2;
static const int ROOT_SKIP = -1;
static const int SAVE_LOAD_LENGTH = 5;
static const int REALLOC_MULTIPLIER = 2;

class TKey {
public:
	TKey();
	virtual ~TKey();

	void Read(char c);
	void Reset();

	bool IsDelimiter(char c);

	char *String;
	short Current;
	short MaxSize;

	void Realloc();
};


TKey::TKey() {

	String = NULL;
	
	Current = 0;
	MaxSize = 0;
}

TKey::~TKey() {

	if (Current) {
		delete[] String;
	}
}


void TKey::Realloc() {

	if (Current == 0) {
		MaxSize = 1;
		String = new char[MaxSize];
	}
	else {
		MaxSize *= REALLOC_MULTIPLIER;
		if (MaxSize > MAX_KEY_LENGTH) {
			MaxSize = MAX_KEY_LENGTH;
		}
		char *tmp = new char[MaxSize];
		int i;
		for (i = 0; i < Current; ++i) {
			tmp[i] = String[i];
		}
		tmp[i] = '\0';

		delete[] String;
		String = tmp;
	}


}

bool TKey::IsDelimiter(char c) {

	return (c == ' ') || (c == '\n') || (c == '\t');
}

void TKey::Read(char c) {

	if (Current == MaxSize) {
		TKey::Realloc();
	}

	String[Current] = c; 
    Current++;
}

void TKey::Reset() {

	this->Current = 0;
	this->MaxSize = 0;

	delete[] this->String;
	this->String = NULL;
}


class TPatriciaNode {
public:
		TPatriciaNode();
		TPatriciaNode(TKey *word, unsigned long long number, short skip, TPatriciaNode *childs[NUMBER_OF_CHILDREN]);
		virtual ~TPatriciaNode(){};

		short BitsToSkip;
		TKey Key;
		unsigned long long Value;
		TPatriciaNode* Links[NUMBER_OF_CHILDREN];
};


class TPatricia {
public:

	TPatricia();
	virtual ~TPatricia();

	bool Remove(TKey *key);
	bool Insert(TKey *key, unsigned long long value);
	TPatriciaNode* Search(TKey *key);

	bool Save(TKey *path);
	bool Load(TKey *path);	

private:

	TPatriciaNode* Root;

	void RemoveTrie(TPatriciaNode *root);
	short GetBit(TKey *key, short bitPosition);
	short GetDifferentBit(TKey *firstWord, TKey *secondWord);
	bool AreDifferent(TKey *firstWord, TKey *secondWord);
	void SwapKeys(TPatriciaNode *first, TPatriciaNode *second);

	void Traverse(FILE *file, TPatriciaNode *root);
};

TPatriciaNode::TPatriciaNode() {
	
	this->Value = 0;
	this->BitsToSkip = ROOT_SKIP;
	this->Links[LEFT_CHILD] = this;
	this->Links[RIGHT_CHILD] = this;
}


TPatriciaNode::TPatriciaNode(TKey *word, unsigned long long number, short skip, TPatriciaNode *childs[NUMBER_OF_CHILDREN]) {
	
	for (int i = 0; word->String[i] != '\0'; ++i) {
		this->Key.Read(word->String[i]);
	}
	this->Key.Read('\0');

	this->Value = number;
	this->BitsToSkip = skip;
	this->Links[LEFT_CHILD] = childs[LEFT_CHILD];
	this->Links[RIGHT_CHILD] = childs[RIGHT_CHILD];
}


TPatricia::TPatricia() {

	Root = new TPatriciaNode();
	Root->Key.Read('\0');
}


TPatricia::~TPatricia(){

	RemoveTrie(this->Root);

}

void TPatricia::RemoveTrie(TPatriciaNode *currentNode) {

	TPatriciaNode *left = currentNode->Links[LEFT_CHILD];
	TPatriciaNode *right = currentNode->Links[RIGHT_CHILD];

	if ((left->BitsToSkip >= currentNode->BitsToSkip) && (left != currentNode) && (left != Root))
		RemoveTrie(left);
	
	if ((right->BitsToSkip >= currentNode->BitsToSkip) && (right != currentNode) && (right != Root))
		RemoveTrie(right);


	delete currentNode;

}



static const int NMEMB = 1;

void TPatricia::Traverse(FILE *dict, TPatriciaNode *currentNode) {

	TPatriciaNode *left = currentNode->Links[LEFT_CHILD];
	TPatriciaNode *right = currentNode->Links[RIGHT_CHILD];

	if ((left->BitsToSkip >= currentNode->BitsToSkip) && (left != currentNode) && (left != Root))
		Traverse(dict, left);
	
	if ((right->BitsToSkip >= currentNode->BitsToSkip) && (right != currentNode) && (right != Root))
		Traverse(dict, right);

	if (currentNode != Root) {

		for (int i = 0; currentNode->Key.String[i] != '\0'; ++i) {
			fwrite(currentNode->Key.String + i, sizeof(char), NMEMB, dict);
		}
		fwrite("\0", sizeof(char), NMEMB, dict);

		fwrite(&currentNode->Value, sizeof(unsigned long long), NMEMB, dict);
	}
}

bool TPatricia::Save(TKey *path) {

	FILE *dict = fopen(path->String, "wb");

	if (dict  == NULL) {
		return false;
	}
	else if (Root->Links[RIGHT_CHILD] == Root) {
		fclose(dict);
		return true;
	}
	else { 
		Traverse(dict, Root);
		fclose(dict);
		return true;
	}
}

bool TPatricia::Load(TKey *path) {

	FILE *dict;

	if ((dict = fopen(path->String, "rb")) == NULL) {
		return false;
	}
	else {
		RemoveTrie(Root);
		Root = new TPatriciaNode();
		Root->Key.Read('\0');

		TKey *buffer = new TKey();
		char character;
		unsigned long long number;

		while (fread(&character, sizeof(char), NMEMB, dict)) {

			for (int i = 0; character != '\0'; i++) {
				buffer->Read(character);
				fread(&character, sizeof(char), NMEMB, dict);
			}
			buffer->Read(character);

			fread(&number, sizeof(unsigned long long), NMEMB, dict);

			Insert(buffer, number);
			buffer->Reset();
		}

		delete buffer;
		fclose(dict);
		return true;
	}

}

bool TPatricia::Insert(TKey *key, unsigned long long value) {
	
	TPatriciaNode *upper, *lower, *tmp;

	upper = Root;
	lower = upper->Links[RIGHT_CHILD];

	while ( upper->BitsToSkip < lower->BitsToSkip) {
		upper = lower;
		if (GetBit(key, lower->BitsToSkip)) {
			lower = lower->Links[RIGHT_CHILD];
		}
		else {
			lower = lower->Links[LEFT_CHILD];
		}
	}

	if (!AreDifferent(key, &lower->Key)) {
		return false;
	}

	short skip = GetDifferentBit(key, &lower->Key);

	upper = Root;
	tmp = Root->Links[RIGHT_CHILD];
	
	while (upper->BitsToSkip < tmp->BitsToSkip  && tmp->BitsToSkip < skip) {
		upper = tmp;
		if (GetBit(key, tmp->BitsToSkip)) {
			tmp = tmp->Links[RIGHT_CHILD];
		}
		else {
			tmp = tmp->Links[LEFT_CHILD];
		}
	}

	TPatriciaNode *childs[NUMBER_OF_CHILDREN];
	childs[LEFT_CHILD] = NULL;
	childs[RIGHT_CHILD] = NULL;

	lower = new TPatriciaNode(key, value, skip, childs);

	if (GetBit(key, skip)) {
		lower->Links[LEFT_CHILD] = tmp;
		lower->Links[RIGHT_CHILD] = lower;
	}
	else {
		lower->Links[LEFT_CHILD] = lower;
		lower->Links[RIGHT_CHILD] = tmp;
	}

	if (GetBit(key, upper->BitsToSkip)) {
		upper->Links[RIGHT_CHILD] = lower;
	}
	else {
		upper->Links[LEFT_CHILD] = lower;
	}
	
	return true;
}

TPatriciaNode *TPatricia::Search(TKey *key) {

	TPatriciaNode *upper;
	TPatriciaNode *lower;

	upper = Root;
	lower = Root->Links[RIGHT_CHILD];

	while (upper->BitsToSkip < lower->BitsToSkip) {
		upper = lower;
		if (GetBit(key, lower->BitsToSkip)) {
			lower = lower->Links[RIGHT_CHILD];
		}
		else {
			lower = lower->Links[LEFT_CHILD];
		}
	}

	if (AreDifferent(key, &lower->Key)) {
		return NULL;
	}

	return lower;

}

bool TPatricia::Remove(TKey *key) {

	TPatriciaNode *upper, *lower, *upperParent;

	upper = Root;
	lower = Root->Links[RIGHT_CHILD];

	while (upper->BitsToSkip < lower->BitsToSkip) {
		upperParent = upper;
		upper = lower;
		if (GetBit(key, lower->BitsToSkip)) {
			lower = lower->Links[RIGHT_CHILD];
		}
		else {
			lower = lower->Links[LEFT_CHILD];
		}
	}

	if (AreDifferent(key, &lower->Key))
		return false; 

	if (upper != lower) {
		SwapKeys(upper, lower);
	}

	short upperSkip = upper->BitsToSkip;
	short leftSkip = upper->Links[LEFT_CHILD]->BitsToSkip;
	short rightSkip = upper->Links[RIGHT_CHILD]->BitsToSkip;

	if ((leftSkip > upperSkip) || (rightSkip > upperSkip)) {
		

		if (upper != lower) {

			int currentLength;
			TKey *word = new TKey();

			for (currentLength = 0; upper->Key.String[currentLength] != '\0'; currentLength++) {
				word->Read(upper->Key.String[currentLength]);
			}
            word->Read('\0');

			TPatriciaNode *upperTmp = upper;
			TPatriciaNode *lowerTmp;

			if (GetBit(word, upper->BitsToSkip)) {
				lowerTmp = upper->Links[RIGHT_CHILD];
			}
			else {
				lowerTmp = upper->Links[LEFT_CHILD];
			}
      
			while (upperTmp->BitsToSkip < lowerTmp->BitsToSkip) {
				upperTmp = lowerTmp;
				if (GetBit(word, lowerTmp->BitsToSkip)) {
					lowerTmp = lowerTmp->Links[RIGHT_CHILD];
				}
				else {
					lowerTmp = lowerTmp->Links[LEFT_CHILD];
				}
			}


            if (AreDifferent(word, &lowerTmp->Key)) {
				return false; 
            }

			if (GetBit(word, upperTmp->BitsToSkip))
				upperTmp->Links[RIGHT_CHILD] = lower;
			else
				upperTmp->Links[LEFT_CHILD] = lower;

		}

		if (upperParent != upper) {

			TPatriciaNode *child;
			if (GetBit(key, upper->BitsToSkip)) {
				child = upper->Links[LEFT_CHILD];
			}
			else {
				child = upper->Links[RIGHT_CHILD];
			}

			if (GetBit(key, upperParent->BitsToSkip)) {
				upperParent->Links[RIGHT_CHILD] = child;
			}
			else {
				upperParent->Links[LEFT_CHILD] = child;
			}
		}

        

	} else {

		
		if (upperParent != upper) {

			TPatriciaNode *upperRight = upper->Links[RIGHT_CHILD];
			TPatriciaNode *upperLeft = upper->Links[LEFT_CHILD];
			if (GetBit(key, upperParent->BitsToSkip)) {

				if ((upperRight == upperLeft) && (upperLeft == upper)) {
					upperParent->Links[RIGHT_CHILD] = upperParent;
				}
				else if (upperLeft == upper) {
					upperParent->Links[RIGHT_CHILD] = upperRight;
				}
				else {
					upperParent->Links[RIGHT_CHILD] = upperLeft;
				}
			}
			else {
			
				if ((upperRight == upperLeft) && (upperLeft == upper)) {
					upperParent->Links[LEFT_CHILD] = upperParent;
				}
				else if (upperLeft == upper) {
					upperParent->Links[LEFT_CHILD] = upperRight;
				}
				else {
					upperParent->Links[LEFT_CHILD] = upperLeft;
				}
			}
		}

	}

	
	delete upper;


	return true;

}

static const int IS_ROOT = 10;
static const int BITS_IN_BYTE = 8;

short TPatricia::GetBit(TKey *key, short position) {

	if (position < 0) {
		return IS_ROOT; 
	}

	int byte = position / BITS_IN_BYTE;
	int bit = position % BITS_IN_BYTE;

	return ((key->String[byte] >> bit) & 0x1);
}

bool TPatricia::AreDifferent(TKey *firstWord, TKey *secondWord) {

	if (!firstWord->String || !secondWord->String) {
       return true;
	}

	int i;
	for (i = 0; i < MAX_KEY_LENGTH && firstWord->String[i] != '\0'; ++i) { 
		if (firstWord->String[i] != secondWord->String[i]) { 
					return true;
		}
	}

	return firstWord->String[i] != secondWord->String[i];
}

short TPatricia::GetDifferentBit(TKey *firstWord, TKey *secondWord) {

	if (!firstWord->String || !secondWord->String) {
       return 0;
	}

	int byte;
	int bit;

	for (byte = 0; firstWord->String[byte] == secondWord->String[byte] && firstWord->String[byte] != '\0'; byte++);
	for (bit = 0; GetBit(firstWord, (byte * BITS_IN_BYTE) + bit) == GetBit(secondWord, (byte * BITS_IN_BYTE) + bit); bit++);

	return (byte * BITS_IN_BYTE) + bit;
}

void TPatricia::SwapKeys(TPatriciaNode	*first, TPatriciaNode *second) {

	if (first != second) {

		second->Key.Reset();

		int i;
		for (i = 0; first->Key.String[i] != '\0'; ++i) {
			second->Key.Read(first->Key.String[i]);
		}
		second->Key.Read('\0');

		second->Value = first->Value;

	}
}


int main(int argc, char* argv[]) {

	TPatricia *patricia = new TPatricia();
	
	char saveLoad[SAVE_LOAD_LENGTH];

	TKey *word = new TKey();
	int character;

	unsigned long long number;

	clock_t insertT = 0;
    clock_t searchT = 0;
    clock_t deleteT = 0;
    clock_t t;

    while ((character = std::cin.get()) != EOF){

    	

    	switch (character) {

    	case '+':

    		t = clock();

    		std::cin >> std::ws;
    		while (! word->IsDelimiter((character = std::cin.get()))) {
    			word->Read(character);
    		}
    		word->Read('\0');
 		
    		std::cin >> number;

    		for (int i = 0; word->String[i] != '\0'; ++i) {
    			if (word->String[i] >= 'A' && word->String[i] <= 'Z') {
    				word->String[i] -= 'A' - 'a';
    			}
    		}

    		if (patricia->Insert(word, number)) {
    			//std::cout << "OK" << std::endl;
    		}
    		else {
    			//std::cout << "Exist" << std::endl;
    		}

    		insertT += clock() - t;
    		break;

    	case '-':

    		t = clock();

    		std::cin >> std::ws;
    		while (! word->IsDelimiter((character = std::cin.get()))) {
    			word->Read(character);
    		}
    		word->Read('\0');

    		for (int i = 0; word->String[i] != '\0'; ++i) {
    			if (word->String[i] >= 'A' && word->String[i] <= 'Z') {
    				word->String[i] -= 'A' - 'a';
    			}
    		}

    		if (patricia->Remove(word)) {
    			//std::cout << "OK" << std::endl;
    		}
    		else {
    			//std::cout << "NoSuchWord" << std::endl;
    		}


            deleteT += clock() - t;
    		break;

    	case '!':

    		std::cin >> saveLoad;
    		std::cin >> std::ws;
    		while (! word->IsDelimiter((character = std::cin.get()))) {
    			word->Read(character);
    		}
    		word->Read('\0');


    		if (saveLoad[0] == 'S') {
    			if (patricia->Save(word)) {
    				//std::cout << "OK" << std::endl;
    			}
    			else {
    				//std::cout << "ERROR: Couldn't create file" << std::endl;
    			}
    		}
    		else {
    			if (patricia->Load(word)) {
    				//std::cout << "OK" << std::endl;
    			}
    			else {
    				//std::cout << "ERROR: Couldn't open file" << std::endl;
    			}
    		}
    		break;

    	default:

    		t = clock();

    		word->Read(character);
    		while (! word->IsDelimiter((character = std::cin.get()))) {
    			word->Read(character);
    		}
    		word->Read('\0');

    		for (int i = 0; word->String[i] != '\0'; ++i) {
    			if (word->String[i] >= 'A' && word->String[i] <= 'Z') {
    				word->String[i] -= 'A' - 'a';
    			}
    		}

    		TPatriciaNode *tmp = patricia->Search(word);

    		if (tmp) {
    			//std::cout << "OK: " << tmp->Value << std::endl;
    		}
    		else {
    			//std::cout << "NoSuchWord" << std::endl;
    		}

    		searchT += clock() - t;
    		break;
    	}

    	word->Reset();
    	std::cin >> std::ws;
    }

    std::cout << "Search time: " << ((float)searchT)/CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << "Delete time: " << ((float)deleteT)/CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << "Insert time: " << ((float)insertT)/CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << "Time at all: " << ((float)searchT)/CLOCKS_PER_SEC + 
                                    ((float)deleteT)/CLOCKS_PER_SEC +
                                    ((float)insertT)/CLOCKS_PER_SEC  << " seconds" << std::endl;


    delete word;
	delete patricia;

	return 0;
}
