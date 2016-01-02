#include "SparseBlock.h"
#include "EncodingConverter.h"

using namespace std;

namespace Common {

	SparseBlock::SparseBlock(const char* text)
	{
		this->text = EncodingConverter::s2ws(text);
	}

	SparseBlock::~SparseBlock()
	{
	}
}