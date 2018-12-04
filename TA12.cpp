#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int cmp = 0;

void FillArray (int *arr, int size) {
    for (int i = 0; i < size; i++ ) {
        arr[i] = rand() % 10;
    }
};

void ShowArray (int *arr,int size) {
    for ( int k = 0; k < size; k++ ) {
        cout << arr[k] << " ";
    }
    cout << endl;
};

bool compare(int a, int b) {
  if (a > b) {
    cmp++;
    return true;
  }
  else {
    cmp++;
    return false;

  }
}

void ShellSort(int *arr, int size){
    int arrM [9] = {1750,701,301,132,57,23,10,4,1};
    int counter = 0;
    for(int k = 0; k < 9; k++) {
        for (int i = k; i <= size; i++){
                int temp = arr[i];
                int j;
                for (j = i; j >= k && compare(arr[j - k], temp); j = j - k){
                arr[j] = arr[j - k];
                counter++;
                }
            arr[j] = temp;
          }
    }
    cout << "counter of swap equal: " << counter << endl;
}

int main () {
    // int size;
    // cout << "Enter array size: ";
    // cin >> size;
    // int *arr = new int [size];
    // FillArray (arr, size);
    int arr [9] = {1750,701,301,132,57,23,10,4,1}ж
    int size = 9;
    ShowArray (arr, size);
    ShellSort (arr, size);
    cout << "--------------------" << endl;
    ShowArray (arr, size);
    cout << "--------------------" << endl;
    cout << " Count of compare is equal: " << cmp << endl;
}
