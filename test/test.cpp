#include <iostream>
#include <vector>

using namespace std;

#pragma pack(push, 1)
struct Pixel
{
	unsigned char R, G, B, A;
};
#pragma pack(pop)

class PixelArray
{
public:
	Pixel& operator[] (int idx)
	{
		return reinterpret_cast<Pixel*>(this)[idx];
	}
};

int main()
{
	unsigned char arr[12] = { '0','1','2','3','4','5','6','7','8','9','A','B' };

	PixelArray& pixels = *reinterpret_cast<PixelArray*>(&arr[0]);

	cout << pixels[0].R << " " << pixels[0].G << " " << pixels[0].A<< " " << endl
		<< pixels[1].R << " " << pixels[1].G << " " << pixels[1].A << " " << endl
		<< pixels[2].R << " " << pixels[2].G << " " << pixels[2].A << " " << endl;

	return 0;
}