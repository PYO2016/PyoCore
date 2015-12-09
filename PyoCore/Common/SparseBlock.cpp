#include "SparseBlock.h"

using namespace std;

namespace PyoCore {

	SparseBlock::SparseBlock(int top, int bottom, int left, int right, const char* text)
	{
		this->top = top;
		this->bottom = bottom;
		this->left = left;
		this->right = right;
		this->text = text;
	}


	SparseBlock::~SparseBlock()
	{
	}
}