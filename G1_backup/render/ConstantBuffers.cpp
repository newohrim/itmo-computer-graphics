#include "ConstantBuffers.h"

#include <cassert>

void ConstantBuffers::CBuf::Set(const std::string& name, const void* data, int bytes)
{
	const auto iter = fieldOffsets.find(name);
	if (iter == fieldOffsets.end()) {
		return;
	}
	const int byteOffset = iter->second;
	void* target = reinterpret_cast<char*>(buffer.data()) + byteOffset;
	memcpy(target, data, bytes);
}

void ConstantBuffers::CBuf::AppendFieldAligned(const std::string& name, int bytes)
{
	assert(fieldOffsets.find(name) == fieldOffsets.end());
	const int bufferAlignedSize = buffer.size() * sizeof(BATCH);
	int offset;
	if (bufferBytes + bytes <= bufferAlignedSize) {
		offset = bufferBytes;
		bufferBytes += bytes;
	} else {
		offset = bufferAlignedSize;
		buffer.resize(buffer.size() + (size_t)std::ceil(bytes / (float)sizeof(BATCH)));
		//offset = (int)((buffer.size() - 1) * sizeof(BATCH));
		bufferBytes = offset + bytes;
	}
	fieldOffsets[name] = offset;
}

void ConstantBuffers::CBuf::GetData(void** ptr, int* size) const
{
	*ptr = (void*)buffer.data();
	*size = buffer.size() * sizeof(BATCH);
}
