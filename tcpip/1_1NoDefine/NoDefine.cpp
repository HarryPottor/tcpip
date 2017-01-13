#include <iostream>
using namespace std;

inline int getMax(int a, int b)
{
	return a > b ? a : b;
}

template<class T>
inline const T& getMin(const T&a, const T&b)
{
	return a < b ? a : b;
}


class MyClass
{
public:
	MyClass() {};
	~MyClass() {};
	static void showSize()
	{
		cout << MYSIZE << endl;
	}
private:
	static const int MYSIZE = 5;
};

int main()
{
	MyClass::showSize();

	cout << "getMax(a, b) = " << getMax(23, 45) << endl;
	cout << "getMin(int a, int b) = " << getMin(12,23) << endl;
	cout << "getMin(char* a, char* b) = " << getMin('a', 'b') << endl;

	system("pause");
	return 0;
}