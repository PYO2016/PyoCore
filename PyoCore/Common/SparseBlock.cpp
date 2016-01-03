#include "SparseBlock.h"

using namespace std;

namespace Common 
{
	SparseBlock::SparseBlock(point p1, point p2, int realArea, std::wstring text)
		: box(p1, p2), text(text), realArea(realArea)
	{

	}

	SparseBlock::~SparseBlock()
	{
	}
}