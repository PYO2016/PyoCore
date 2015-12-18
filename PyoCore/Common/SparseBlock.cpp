#include "SparseBlock.h"
#include "EncodingConverter.h"

using namespace std;

namespace Common {

	SparseBlock::SparseBlock(int top, int bottom, int left, int right, const char* text)
	{
		this->top = top;
		this->bottom = bottom;
		this->left = left;
		this->right = right;
		this->text = EncodingConverter::s2ws(text);
	}


	SparseBlock::~SparseBlock()
	{
	}
}