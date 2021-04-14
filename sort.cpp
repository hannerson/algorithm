#include<iostream>
#include<vector>

using namespace std;

void bubbleSort(vector<int>& nums){
    bool didswap = false;
    for(int i=0;i<nums.size();i++){
        didswap = false;
        for(int j=0;j<nums.size()-i-1;j++){
            if(nums[j] > nums[j+1]){
                swap(nums[j], nums[j+1]);
                didswap = true;
            }
        }
        if(didswap == false){
            return;
        }
    }
}

void selectSort(vector<int>& nums){
    for(int i=0;i<nums.size();i++){
        int index_min = i;
        for(int j=i+1;j<nums.size();j++){
            if(nums[index_min] > nums[j]){
                index_min = j;
            }
        }
        swap(nums[i], nums[index_min]);
    }
}

void insertSort(vector<int>& nums){
    for(int i=1;i<nums.size();i++){
        for(int j=i;j>0;j--){
            if(nums[j] < nums[j-1]){
                swap(nums[j], nums[j-1]);
            }else{
                break;
            }
        }
    }
}

void insertSort2(vector<int>& nums){
    for(int i=1;i<nums.size();i++){
        int pre = i - 1;
        int j = i;
        int pivot = nums[i];
        while(pre >=0 && nums[pre] > pivot){
            nums[j] = nums[pre];
            pre --;
            j --;
        }
        nums[j] = pivot;
    }
}

void mergeSort(vector<int>& nums, int left, int right){
    if(left < right){
        int mid = (left+right)/2;
        mergeSort(nums, left, mid);
        mergeSort(nums, mid+1, right);
        //merge
        vector<int> result;
        //cout << left << "," << right;
        int i=left,m=mid,j=mid+1,n=right;
        while(i <= m && j <= n){
            if(nums[i] < nums[j]){
                result.push_back(nums[i]);
                i++;
            }else{
                result.push_back(nums[j]);
                j++;
            }
        }
        while(i <= m){
            result.push_back(nums[i]);
            i++;
        }
        while(j <= n){
            result.push_back(nums[j]);
            j++;
        }
        for(int k=left;k<=right;k++){
            nums[k] = result[k-left];
        }
    }
}

void quickSort(vector<int>& nums, int left, int right){
    int i = left, j = right;
    int pivot = nums[i];
    while(i < j){
        while(i < j && pivot <= nums[j]){
            j--;
        }
        nums[i] = nums[j];
        while(i < j && pivot >= nums[i]){
            i++;
        }
        nums[j] = nums[i];
    }
    nums[i] = pivot;
    if(left < i-1){
        quickSort(nums, left, i-1);
    }
    if(right > i+1){
        quickSort(nums, i+1, right);
    }
}


void adjustHeap(vector<int> &nums, int i, int n){
    //int i = 0;
    while(i <= n){
        int l = 2*i+1;
        int r = 2*i+2;
        int k = i;
        if(l <= n && r <= n){
            //int k = i;
            if(nums[l] > nums[r]){
                k = l;
            }else{
                k = r;
            }
            if(k != i && nums[k] > nums[i]){
                swap(nums[i], nums[k]);
            }
        }else if(l <= n){
            if(nums[l] > nums[i]){
                swap(nums[l], nums[i]);
                k = l;
            }
        }
        if(k == i){
            break;
        }else{
            i = k;
        }
    }
}

void buildHeap(vector<int> &nums){
    int n = nums.size()-1;
    int i = (n-1)/2;
    while(i>=0){
        adjustHeap(nums, i, n);
        i--;
    }
}

vector<int> sortHeap(vector<int> &nums){
    //1.build heap top big
    buildHeap(nums);
    for(int i=0;i<nums.size();i++){
        cout << nums[i] << "," ;
    }
    //2.top and swap
    //3.adjust heap
    int n = nums.size();
    for(int i=0;i<n;i++){
        swap(nums[0], nums[n-i-1]);
        adjustHeap(nums, 0, n-i-2);
    }
    return nums;
}

int main(){
	vector<int> s{4,3,1,2,4,6,4,3,4}, s1;
	for(int i=0;i<s.size();i++){
		cout << s[i] << "-" ;
	}
    cout << endl;
    s1.assign(s.begin(), s.end());
    bubbleSort(s1);
	for(int i=0;i<s1.size();i++){
		cout << s1[i] << "-" ;
	}
    cout << endl;
    s1.assign(s.begin(), s.end());
    selectSort(s1);
	for(int i=0;i<s1.size();i++){
		cout << s1[i] << "-" ;
	}
    cout << endl;
    s1.assign(s.begin(), s.end());
    insertSort2(s1);
	for(int i=0;i<s1.size();i++){
		cout << s1[i] << "-" ;
	}
    cout << endl;
    s1.assign(s.begin(), s.end());
    mergeSort(s1, 0, s1.size()-1);
	for(int i=0;i<s1.size();i++){
		cout << s1[i] << "-" ;
	}
    cout << endl;
}
