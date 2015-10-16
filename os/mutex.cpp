#include <condition_variable>
#include <mutex>
#include <future>
#include <thread>
#include <iostream>
#include <queue>
using namespace std;

queue<int> queue1;
mutex queueMutex;
condition_variable queueCondVar;

void provider(int val)
{
	for (size_t i = 0; i < 6; i++)
	{
		lock_guard<mutex> lg(queueMutex);
		queue1.push(val + i);
	}
	queueCondVar.notify_one();

	this_thread::sleep_for(chrono::milliseconds(val));
}

void consumenr(int num)
{
	while (true)
	{
		int val;
		{
			unique_lock<mutex> ul(queueMutex);
			queueCondVar.wait(ul, [] {return !queue1.empty(); });
			val = queue1.front();
			queue1.pop();
		}
		cout << "consumer " << num << ": " << val << endl;
	} 
}

int main()
{
	auto p1 = async(launch::async, provider, 100);
	auto p2 = async(launch::async, provider, 300);
	auto p3 = async(launch::async, provider, 500);

	auto c1 = async(launch::async, consumenr, 1);
	auto c2 = async(launch::async, consumenr, 2);
	return 0;
}