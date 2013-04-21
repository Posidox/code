#include<iostream>
using namespace std;


template<size_t N>
struct fact
{
	static const size_t Result = fact<N-1>::Result * N;
};

template<>
struct fact<0>
{
	static const size_t Result = 1;
};


int main()
{
	std::cout<<fact<10>::Result<<endl;
	return 0;
}
