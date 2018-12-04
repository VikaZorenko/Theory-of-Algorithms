#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <string>
#include <cmath>
#include <vector>
using namespace std;

int print_counter = 0;
int comparison_counter = 0;

int Search_Binary(int arr[], int left, int right, int key) {
	int midd = 0;
	while (1) {
		midd = (left + right) / 2;
		if (key < arr[midd]) {
			comparison_counter++;
			right = midd - 1;
		} else if (key > arr[midd]) {
			comparison_counter++;
			left = midd + 1;
		} else  {
			comparison_counter++;
			return midd;
		}                    
		if (left > right)         
			return -1;
	}
}

class BTreeNode
{
	int *keys; // An array of keys
	int *values; // An array of values
	int t;	 // Minimum degree (defines the range for number of keys)
	BTreeNode **C; // An array of child pointers
	int n;	 // Current number of keys
	bool leaf; // Is true when node is leaf. Otherwise false

public:
	BTreeNode(int _t, bool _leaf); // Constructor
	void traverse();
	int searchValueByKey(int k);
	int searchIndexOfKey(int k);
	BTreeNode *searchNodeByKey(int k);
	int findKey(int k);
	void insertNonFull(int k, int v);
	void insertNonFullFromFile(int k, int v);
	void splitChild(int i, BTreeNode *y);
	void remove(int k);
	void removeFromLeaf(int idx);
	void removeFromNonLeaf(int idx);
	int getPred(int idx);
	int getSucc(int idx);
	void fill(int idx);
	void borrowFromPrev(int idx);
	void borrowFromNext(int idx);
	void merge(int idx);
	friend class BTree;
};

class BTree
{
	BTreeNode *root; // Pointer to root node
	int t; // Minimum degree
public:
	vector<int> treeKeys;
	vector<int> treeValues;
	BTree(int _t) {
		root = NULL;
		t = _t;
	}
	void traverse() {
		if (root != NULL) root->traverse();
	}
	int searchValueByKey(int k) {
		return (root == NULL) ? 0 : root->searchValueByKey(k);
	}
	int searchIndexOfKey(int k) {
		return (root == NULL) ? 0 : root->searchIndexOfKey(k);
	}
	BTreeNode* searchNodeByKey(int k) {
		return (root == NULL) ? NULL : root->searchNodeByKey(k);
	}
	void insert(int k, int v);
	void insertFromFile(int k, int v);
	void remove(int k);
	void change(int k, int v);

};

BTreeNode::BTreeNode(int t1, bool leaf1) {
	t = t1;
	leaf = leaf1;
	keys = new int[2 * t - 1];
	values = new int[2 * t - 1];
	C = new BTreeNode *[2 * t];
	n = 0;
}

int BTreeNode::findKey(int k) {
	int idx = 0;
	while (idx<n && keys[idx] < k)
		++idx;
	return idx;
}

void BTreeNode::remove(int k) {
	int idx = findKey(k);
	if (idx < n && keys[idx] == k) {
		if (leaf)
			removeFromLeaf(idx);
		else
			removeFromNonLeaf(idx);
	} else {
		if (leaf) {
			cout << "The key " << k << " is does not exist in the tree\n";
			return;
		}
		bool flag = ((idx == n) ? true : false);
		if (C[idx]->n < t)
			fill(idx);
		if (flag && idx > n)
			C[idx - 1]->remove(k);
		else
			C[idx]->remove(k);
	}
	return;
}

void BTreeNode::removeFromLeaf(int idx) {
	for (int i = idx + 1; i < n; ++i) {
		keys[i - 1] = keys[i];
		values[i - 1] = values[i];
	}
	n--;
	return;
}

void BTreeNode::removeFromNonLeaf(int idx) {
	int k = keys[idx];
	if (C[idx]->n >= t) {
		int pred = getPred(idx);
		keys[idx] = pred;
		C[idx]->remove(pred);
	} else if (C[idx + 1]->n >= t) {
		int succ = getSucc(idx);
		keys[idx] = succ;
		C[idx + 1]->remove(succ);
	} else {
		merge(idx);
		C[idx]->remove(k);
	}
	return;
}

int BTreeNode::getPred(int idx) {
	BTreeNode *cur = C[idx];
	while (!cur->leaf)
		cur = cur->C[cur->n];
	return cur->keys[cur->n - 1];
}

int BTreeNode::getSucc(int idx) {
	BTreeNode *cur = C[idx + 1];
	while (!cur->leaf)
		cur = cur->C[0];
	return cur->keys[0];
}

void BTreeNode::fill(int idx) {
	if (idx != 0 && C[idx - 1]->n >= t)
		borrowFromPrev(idx);
	else if (idx != n && C[idx + 1]->n >= t)
		borrowFromNext(idx);
	else {
		if (idx != n)
			merge(idx);
		else
			merge(idx - 1);
	}
	return;
}

void BTreeNode::borrowFromPrev(int idx) {
	BTreeNode *child = C[idx];
	BTreeNode *sibling = C[idx - 1];
	for (int i = child->n - 1; i >= 0; --i) {
		child->keys[i + 1] = child->keys[i];
		child->values[i + 1] = child->values[i];
	}
	if (!child->leaf) {
		for (int i = child->n; i >= 0; --i)
			child->C[i + 1] = child->C[i];
	}
	child->keys[0] = keys[idx - 1];
	child->values[0] = values[idx - 1];
	if (!leaf)
		child->C[0] = sibling->C[sibling->n];
	keys[idx - 1] = sibling->keys[sibling->n - 1];
	values[idx - 1] = sibling->values[sibling->n - 1];
	child->n += 1;
	sibling->n -= 1;
	return;
}

void BTreeNode::borrowFromNext(int idx) {
	BTreeNode *child = C[idx];
	BTreeNode *sibling = C[idx + 1];
	child->keys[(child->n)] = keys[idx];
	child->values[(child->n)] = values[idx];
	if (!(child->leaf))
		child->C[(child->n) + 1] = sibling->C[0];
	keys[idx] = sibling->keys[0];
	values[idx] = sibling->values[0];
	for (int i = 1; i < sibling->n; ++i) {
		sibling->keys[i - 1] = sibling->keys[i];
		sibling->values[i - 1] = sibling->values[i];
	}
	if (!sibling->leaf) {
		for (int i = 1; i <= sibling->n; ++i)
			sibling->C[i - 1] = sibling->C[i];
	}
	child->n += 1;
	sibling->n -= 1;
	return;
}

void BTreeNode::merge(int idx) {
	BTreeNode *child = C[idx];
	BTreeNode *sibling = C[idx + 1];
	child->keys[t - 1] = keys[idx];
	child->values[t - 1] = values[idx];
	for (int i = 0; i < sibling->n; ++i) {
		child->keys[i + t] = sibling->keys[i];
		child->values[i + t] = sibling->values[i];
	}
	if (!child->leaf) {
		for (int i = 0; i <= sibling->n; ++i)
			child->C[i + t] = sibling->C[i];
	}
	for (int i = idx + 1; i < n; ++i) {
		keys[i - 1] = keys[i];
		values[i - 1] = values[i];
	}
	for (int i = idx + 2; i <= n; ++i)
		C[i - 1] = C[i];
	child->n += sibling->n + 1;
	n--;
	delete(sibling);
	return;
}

void BTree::insert(int k, int v) {
	for (unsigned int y = 0; y < treeKeys.size(); y++) {
		if (k == treeKeys[y]) {
			k++;
		}
	}
	treeKeys.push_back(k);
	treeValues.push_back(v);
	if (root == NULL) {
		root = new BTreeNode(t, true);
		root->keys[0] = k; // Insert key
		root->values[0] = v; // Insert value
		root->n = 1; // Update number of keys in root
		ofstream out;
		out.open("lab3TA.txt", ios::app);
		if (out.is_open()) {
			out << k << " " << v << endl;
		}
		out.close();
	} else {
		if (root->n == 2 * t - 1) {
			BTreeNode *s = new BTreeNode(t, false);
			s->C[0] = root;
			s->splitChild(0, root);
			int i = 0;
			if (s->keys[0] < k)
				i++;
			s->C[i]->insertNonFull(k, v);
			root = s;
		}
		else 
			root->insertNonFull(k, v);
	}
}

void BTreeNode::insertNonFull(int k, int v) {
	for (int f = 0; f < n; f++) {
		if (keys[f] == k) {
			k++;
		}
	}
	int i = n - 1;
	if (leaf == true) {
		while (i >= 0 && keys[i] > k) {
			keys[i + 1] = keys[i];
			values[i + 1] = values[i];
			i--;
		}
		keys[i + 1] = k;
		values[i + 1] = v;
		n = n + 1;
		ofstream out;
		out.open("lab3TA.txt", ios::app);
		if (out.is_open()) {
			out << k << " " << v << endl;
		}
		out.close();
	} else {
		while (i >= 0 && keys[i] > k)
			i--;
		if (C[i + 1]->n == 2 * t - 1) {
			splitChild(i + 1, C[i + 1]);
			if (keys[i + 1] < k)
				i++;
		}
		C[i + 1]->insertNonFull(k, v);
	}
}

void BTree::insertFromFile(int k, int v) {
	for (unsigned int y = 0; y < treeKeys.size(); y++) {
		if (k == treeKeys[y]) {
			k++;
		}
	}
	treeKeys.push_back(k);
	treeValues.push_back(v);
	if (root == NULL) {
		root = new BTreeNode(t, true);
		root->keys[0] = k; // Insert key
		root->values[0] = v; // Insert value
		root->n = 1; // Update number of keys in root
	} else  {
	if (root->n == 2 * t - 1) {
			BTreeNode *s = new BTreeNode(t, false);
			s->C[0] = root;
			s->splitChild(0, root);
			int i = 0;
			if (s->keys[0] < k)
				i++;
			s->C[i]->insertNonFullFromFile(k, v);
			root = s;
		}
		else 
			root->insertNonFullFromFile(k, v);
	}
}

void BTreeNode::insertNonFullFromFile(int k, int v) {
	for (int f = 0; f < n; f++) {
		if (keys[f] == k) {
			k++;
		}
	}
	int i = n - 1;
	if (leaf == true) {
		while (i >= 0 && keys[i] > k) {
			keys[i + 1] = keys[i];
			values[i + 1] = values[i];
			i--;
		}
		keys[i + 1] = k;
		values[i + 1] = v;
		n = n + 1;
	} else  {
		while (i >= 0 && keys[i] > k)
			i--;
		if (C[i + 1]->n == 2 * t - 1) {
			splitChild(i + 1, C[i + 1]);
			if (keys[i + 1] < k)
				i++;
		}
		C[i + 1]->insertNonFullFromFile(k, v);
	}
}

void BTreeNode::splitChild(int i, BTreeNode *y) {
	BTreeNode *z = new BTreeNode(y->t, y->leaf);
	z->n = t - 1;
	for (int j = 0; j < t - 1; j++) {
		z->keys[j] = y->keys[j + t];
		z->values[j] = y->values[j + t];
	}
	if (y->leaf == false) {
		for (int j = 0; j < t; j++)
			z->C[j] = y->C[j + t];
	}
	y->n = t - 1;
	for (int j = n; j >= i + 1; j--)
		C[j + 1] = C[j];
	C[i + 1] = z;
	for (int j = n - 1; j >= i; j--) {
		keys[j + 1] = keys[j];
		values[j + 1] = values[j];
	}
	keys[i] = y->keys[t - 1];
	values[i] = y->values[t - 1];
	n = n + 1;
}

void BTreeNode::traverse() {
	int i;
	for (i = 0; i < n; i++) {
		if (leaf == false)
			C[i]->traverse();
		cout << " " << keys[i];
	}
	if (leaf == false)
		C[i]->traverse();
}

int BTreeNode::searchValueByKey(int k) {
	if (Search_Binary(keys, 0, n - 1, k) != -1) {
		return values[Search_Binary(keys, 0, n - 1, k)];
	} else {
		int i = 0;
		while (i < n && k > keys[i])
			i++;
		if (leaf == true)
			return 0;
		return C[i]->searchValueByKey(k);
	}
}

int BTreeNode::searchIndexOfKey(int k) {
	if (Search_Binary(keys, 0, n - 1, k) != -1) {
		comparison_counter = 0;
		return Search_Binary(keys, 0, n - 1, k);
	} else {
		int i = 0;
		while (i < n && k > keys[i])
			i++;
		if (leaf == true)
			return -1;
		return C[i]->searchIndexOfKey(k);
	}
}

BTreeNode *BTreeNode::searchNodeByKey(int k) {
	int i = 0;
	while (i < n && k > keys[i])
		i++;
	if (keys[i] == k)
		return this;
	if (leaf == true)
		return NULL;
	return C[i]->searchNodeByKey(k);
}

void BTree::remove(int k) {
	if (!root) {
		cout << "The tree is empty\n";
		return;
	}
	if (searchIndexOfKey(k) != -1) {
		unsigned int size1 = treeKeys.size();
		unsigned int size2 = treeValues.size();
		int arr1[12000] = { 0 };
		for (unsigned int i = 0; i < size1; i++) {
			arr1[i] = treeKeys[i];
		}
		int arr2[12000] = { 0 };
		for (unsigned int j = 0; j < size2; j++) {
			arr2[j] = treeValues[j];
		}
		arr2[Search_Binary(arr1, 0, size2 - 1, k)] = -1;
		arr1[Search_Binary(arr1, 0, size1 - 1, k)] = -1;
		comparison_counter = 0;
		treeKeys.clear();
		treeValues.clear();
		for (unsigned int r = 0; r < size1; r++) {
			if (arr1[r] != -1) {
				treeKeys.push_back(arr1[r]);
			}
		}
		for (unsigned int e = 0; e < size1; e++) {
			if (arr2[e] != -1) {
				treeValues.push_back(arr2[e]);
			}
		}
		print_counter = 0;
		ofstream out;
		out.open("lab3TA.txt");
		if (out.is_open()) {
			out << "";
		}
		out.close();
		for (unsigned int u = 0; u < treeKeys.size(); u++) {
			out.open("lab3TA.txt", ios::app);
			if (out.is_open())
			{
				out << treeKeys[u] << " " << treeValues[u] << endl;
			}
			out.close();
		}
		cout << "Key is deleted" << endl;
	}
	root->remove(k);
	if (root->n == 0) {
		BTreeNode *tmp = root;
		if (root->leaf)
			root = NULL;
		else
			root = root->C[0];
		delete tmp;
	}
	return;
}

void BTree::change(int k, int v) {
	if (searchIndexOfKey(k) != -1) {
		searchNodeByKey(k)->values[searchIndexOfKey(k)] = v;

		int arr[12000] = { 0 };
		for (unsigned int i = 0; i < treeKeys.size(); i++) {
			arr[i] = treeKeys[i];
		}
		if (Search_Binary(arr, 0, treeKeys.size() - 1, k)) {
			treeValues[Search_Binary(arr, 0, treeKeys.size() - 1, k)] = v;
		}
		comparison_counter = 0;
		print_counter = 0;
		ofstream out;
		out.open("lab3TA.txt");
		if (out.is_open()) {
			out << "";
		}
		out.close();
		for (unsigned int u = 0; u < treeKeys.size(); u++) {
			out.open("lab3TA.txt", ios::app);
			if (out.is_open()) {
				out << treeKeys[u] << " " << treeValues[u] << endl;
			}
			out.close();
		}
	} else {
		cout << "Key is not found" << endl;
	}
}

int main() {
	int num1;
	cout << "1 - Read from file" << endl;
	cout << "2 - Create new tree" << endl;
	cout << "3 - Exit" << endl;
	cout << endl;
	cout << "Your choice: ";
	cin >> num1;
	if (num1 == 1) {
		BTree t(1000);
		ifstream fin("lab3TA.txt");
		int key = 0, key_tmp;
		int value = 0, value_tmp;
		if (fin.is_open()) {
			while (fin >> key_tmp && fin >> value_tmp) {
				key = key_tmp;
				value = value_tmp;
				t.insertFromFile(key, value);
			}
		}
		fin.close();
		int num2;
		cout << "B-tree was read from file" << endl;
		cout << "Choose one of these options to continue: " << endl;
		cout << "1 - Search value by the key" << endl;
		cout << "2 - Delete key" << endl;
		cout << "3 - Change the value of key" << endl;
		cout << "4 - Add pair (key, value) to the tree" << endl;
		cout << "5 - Exit" << endl;
		cout << endl;
		cout << "Your choice: ";
		cin >> num2;
		while (num2 != 5) {
			if (num2 == 1) {
				int keyToSearch;
				cout << "Enter key: ";
				cin >> keyToSearch;
				cout << "Value of this key: " << t.searchValueByKey(keyToSearch) << endl;
				cout << "Comparisons: " << comparison_counter << endl;
				cout << endl;
				keyToSearch = -1;
				comparison_counter = 0;
				num2 = 0;
				cout << "Your choice: ";
				cin >> num2;
			}
			if (num2 == 2) {
				int keyToDelete;
				cout << "Enter key: ";
				cin >> keyToDelete;
				t.remove(keyToDelete);
				cout << endl;
				keyToDelete = -1;
				num2 = 0;
				cout << "Your choice: ";
				cin >> num2;
			}
			if (num2 == 3) {
				int keyToChange;
				int valueToChange;
				cout << "Enter key: ";
				cin >> keyToChange;
				cout << "Enter new value: ";
				cin >> valueToChange;
				t.change(keyToChange, valueToChange);
				cout << endl;
				keyToChange = -1;
				valueToChange = -1;
				num2 = 0;
				cout << "Your choice: ";
				cin >> num2;
			}
			if (num2 == 4) {
				int keyToAdd;
				int valueToAdd;
				cout << "If you enter key that is already situated in tree, this key will be incrementing while it will be unique" << endl;
				cout << "Enter key: ";
				cin >> keyToAdd;
				cout << "Enter value: ";
				cin >> valueToAdd;
				t.insert(keyToAdd, valueToAdd);
				cout << "New key and value were added to tree" << endl;
				cout << endl;
				keyToAdd = -1;
				valueToAdd = -1;
				num2 = 0;
				cout << "Your choice: ";
				cin >> num2;
			}
			while ((num2 < 1) || (num2 > 5)) {
				num2 = 0;
				cout << "Enter from 1 to 5: ";
				cin >> num2;
			}
		}
		if (num2 == 5) {
			num2 = 0;
			return 0;
		}
	}
	if (num1 == 2) {
		ofstream out;
		out.open("lab3TA.txt");
		if (out.is_open()) {
			out << "";
		}
		out.close();
		BTree t(1000); 
		for (int i = 0; i < 10000; i++) {
			t.insert(i, rand() % 20000);
		}
		int num2;
		cout << "B-tree with t = 1000 parametr and 10000 keys and values was generated." << endl;
		cout << "Choose one of these options to continue: " << endl;
		cout << "1 - Search value by the key" << endl;
		cout << "2 - Delete key" << endl;
		cout << "3 - Change the value of key" << endl;
		cout << "4 - Add pair (key, value) to the tree" << endl;
		cout << "5 - Exit" << endl;
		cout << endl;
		cout << "Your choice: ";
		cin >> num2;
		while (num2 != 5) {
			if (num2 == 1) {
				int keyToSearch;
				cout << "Enter key: ";
				cin >> keyToSearch;
				cout << "Value of this key: " << t.searchValueByKey(keyToSearch) << endl;
				cout << "Comparisons: " << comparison_counter << endl;
				cout << endl;
				keyToSearch = -1;
				comparison_counter = 0;
				num2 = 0;
				cout << "Your choice: ";
				cin >> num2;
			}
			if (num2 == 2) {
				int keyToDelete;
				cout << "Enter key: ";
				cin >> keyToDelete;
				t.remove(keyToDelete);
				cout << endl;
				keyToDelete = -1;
				num2 = 0;
				cout << "Your choice: ";
				cin >> num2;
			}
			if (num2 == 3) {
				int keyToChange;
				int valueToChange;
				cout << "Enter key: ";
				cin >> keyToChange;
				cout << "Enter new value: ";
				cin >> valueToChange;
				t.change(keyToChange, valueToChange);
				cout << endl;
				keyToChange = -1;
				valueToChange = -1;
				num2 = 0;
				cout << "Your choice: ";
				cin >> num2;
			}
			if (num2 == 4) {
				int keyToAdd;
				int valueToAdd;
				cout << "If you enter key that is already situated in tree, this key will be incrementing while it will be unique" << endl;
				cout << "Enter key: ";
				cin >> keyToAdd;
				cout << "Enter value: ";
				cin >> valueToAdd;
				t.insert(keyToAdd, valueToAdd);
				cout << "New key and value were added to tree" << endl;
				cout << endl;
				keyToAdd = -1;
				valueToAdd = -1;
				num2 = 0;
				cout << "Your choice: ";
				cin >> num2;
			}
			while ((num2 < 1) || (num2 > 5)) {
				num2 = 0;
				cout << "Enter from 1 to 5: ";
				cin >> num2;
			}
		}
		if (num2 == 5) {
			num2 = 0;
			return 0;
		}
	}
	if (num1 == 3) {
		return 0;
	}
	while ((num1 < 1) || (num1 > 3)) {
		cout << "Enter number from 1 to 3: ";
		cin >> num1;
	}
}