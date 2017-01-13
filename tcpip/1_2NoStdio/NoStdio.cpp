#include <iostream>

using namespace std;
class FenShu
{
public:
	FenShu(int fz, int fm):fz(fz), fm(fm){ }

	friend ostream &  operator<<(ostream &o, FenShu& fs)
	{

		return o << fs.fz << "/" << fs.fm;
	}
private:
	int fz;
	int fm;
};


int main()
{
	FenShu fs(1, 2);
	FenShu *fs2;
	fs2 = new FenShu(4, 5);

	cout << fs << *fs2 << endl;

	system("pause");
	return 0;
}