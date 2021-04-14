#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
#include <condition_variable>
#include <functional>
#include <memory>
#include <queue>
#include <atomic>
#include <chrono>
#include <tuple>

using namespace std;

class threadpool{
	public:
		using Task = function<void()>;
		threadpool(int num):_thread_num(num),_task_num(0),_is_stop(false){
		}
		~threadpool(){
		}
		template<typename T, typename ... Args>
		void addTask(T && t, Args && ... args){
			unique_lock<mutex> lock(_mutex);
			Task task = bind(forward<T>(t),forward<Args>(args)...);
			_tasks.push(task);
			_cond.notify_one();
            _task_num ++;
		}

		void stop(){
			_is_stop = true;
			{
				unique_lock<mutex> lock(_mutex);
				_cond.notify_all();
			}
			for(auto &t: _threads ){
				t.join();
			}
		}

		void start(){
			for(int i=0;i<_thread_num;i++){
				_threads.emplace_back(thread(&threadpool::worker, this));
			}
		}

		void worker(){
			while(!_is_stop){
				Task task;
				{
					unique_lock<mutex> lock(_mutex);
					if(!_tasks.empty()){
						task = _tasks.front();
						_tasks.pop();
					}else{
						_cond.wait(lock);
						if(!_tasks.empty()){
							task = _tasks.front();
							_tasks.pop();
						}
					}
				}
				if(task){
					task();
                    unique_lock<mutex> lock(_mutex_over);
                    _task_processed_num ++;
                    _cond_task_over.notify_one();
				}
			}
		}

        void taskOver(){
            unique_lock<mutex> lock(_mutex_over);
            while(_task_processed_num < _task_num){
                _cond_task_over.wait(lock);
            }
            lock.unlock();
        }

        size_t taskLeft(){
            size_t num = 0;
            {
			    unique_lock<mutex> lock(_mutex);
                num = _tasks.size();
            }
            return num;
        }
	private:
		atomic<bool> _is_stop;
		int _thread_num;
		mutex _mutex;
		condition_variable _cond;
		vector<thread> _threads;
		queue<Task> _tasks;
        mutex _mutex_over;
        condition_variable _cond_task_over;
        size_t _task_processed_num;
        size_t _task_num;
};

void sortQuick(vector<int> &nums, int begin, int end, threadpool *pool){
	if(begin >= end){
		return;
	}
	int val = nums[begin];
	int i = begin, j = end;
	while(i < j){
		while(i < j && nums[j] > val){
			j --;
		}
		//cout << i << "-" << j << endl;
		if(i < j){
			nums[i] = nums[j];
			i++;
		}
		while(i < j && nums[i] < val){
			i++;
		}
		if(i < j){
			nums[j] = nums[i];
			j--;
		}
	}
	nums[i] = val;
	//cout << pool << endl;
	pool->addTask(sortQuick, ref(nums), begin, i-1, pool);
	pool->addTask(sortQuick, ref(nums), i+1, end, pool);
	//sortQuick(nums, begin, i-1, pool);
	//sortQuick(nums, i+1, end, pool);
}

void mergeSortPool(vector<int>& nums, int left, int right, threadpool *pool){
    if(left < right){
        int mid = (left+right)/2;
        //cout << pool << endl;
        pool->addTask(mergeSortPool, ref(nums), left, mid, pool);
        pool->addTask(mergeSortPool, ref(nums), mid+1, right, pool);
        //mergeSort(nums, left, mid);
        //mergeSort(nums, mid+1, right);
        //merge
        vector<int> result;
        cout << left << "," << right << endl;
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

void mergeSort(vector<int>& nums, int left, int right){
    if(left < right){
        int mid = (left+right)/2;
        mergeSort(nums, left, mid);
        mergeSort(nums, mid+1, right);
        //merge
        vector<int> result;
        //cout << left << "," << right << endl;
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

void mergeSortThread(vector<int>& nums, threadpool *pool){
    int div_num = 3;
    vector<pair<int,int>> index;
    for(int i=0; i<div_num;i++){
        int begin = (nums.size()*i)/div_num;
        int end = (nums.size()*(i+1))/div_num - 1;
        index.push_back(pair<int,int>(begin, end));
        pool->addTask(mergeSort, ref(nums), begin, end);
    }
    //wait task over
    pool->taskOver();
    cout << pool->taskLeft() << endl;

	for(int i=0;i<nums.size();i++){
		cout << nums[i] << "-" ;
	}
	cout << endl;
    //merge 
    vector<int> result;
    bool over = false;
    while(!over){
        over = true;
        int min = 0;
        int m_idx = 0;
        for(int i=0; i<div_num;i++){
            if(index[i].first <= index[i].second){
                min = index[i].first;
                m_idx = i;
                break;
            }
        }
        for(int i=0; i<div_num; i++){
            if(index[i].first <= index[i].second){
                over = false;
                if(nums[min] > nums[index[i].first]){
                    min = index[i].first;
                    m_idx = i;
                }
            }
        }
        index[m_idx].first ++;
        result.push_back(nums[min]);
    }
    for(int k=0;k<nums.size();k++){
        nums[k] = result[k];
    }
}

int main(){
	vector<int> s{4,3,1,2,4,6,4,3,4};
	for(int i=0;i<s.size();i++){
		cout << s[i] << "-" ;
	}
	cout << endl;
	threadpool *pool = new threadpool(5);
	pool->start();
	pool->addTask(sortQuick, ref(s), 0, 8, pool);
	this_thread::sleep_for(chrono::milliseconds(2000));
	for(int i=0;i<s.size();i++){
		cout << s[i] << "-" ;
	}
	cout << endl;
	vector<int> s1{4,3,1,2,7,8,10,23,4,6,4,3,4};
	for(int i=0;i<s1.size();i++){
		cout << s1[i] << "-" ;
	}
	cout << endl;
    mergeSortThread(s1, pool);
	for(int i=0;i<s1.size();i++){
		cout << s1[i] << "-" ;
	}
	cout << endl;
	pool->stop();
}
