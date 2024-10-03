#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#define CB_DECLARE_BEGIN(CB_NAME)																			\
	struct CB_NAME : public ConstantBuffers::CBuf {															\
		CB_NAME() { Init(); }																				\
																											\
		static int GetBufferSize() 																			\
		{																									\
			CB_NAME cbuf;																					\
			void* ptr;																						\
			int size;																						\
			cbuf.GetData(&ptr, &size);																		\
			return size;																					\
		}																									\
																											\
		void Init() override																				\
		{																									\

#define CB_DECLARE_FIELD(TYPE, NAME)																		\
			AppendFieldAligned(#NAME, sizeof(TYPE));														\

#define CB_DECLARE_ARRAY(TYPE, NAME, SIZE)																	\
			AppendFieldAligned(#NAME, sizeof(TYPE) * SIZE);													\

#define CB_DECLARE_END()																					\
		}																									\
	};																										\

#define CB_SET_FIELD(CBUF, VAR)																				\
	CBUF.Set(#VAR, &VAR)																					\

#define CB_SET_ARRAY(CBUF, VAR, SIZE)																		\
	CBUF.SetArray(#VAR, &VAR, SIZE)																			\

#define CB_GET_FIELD(CBUF, VAR)																				\
	CBUF.Get(#VAR, &VAR)																					\

#define CB_GET_PTR(CBUF, VAR)																				\
	CBUF.GetPtr(#VAR, &VAR)																					\

class ConstantBuffers {
public:
	struct CBuf {
		template<typename T>
		void Set(const std::string& name, T* data) { Set(name, data, sizeof(T)); }

		template<typename T>
		void SetArray(const std::string& name, T* data, int size) { Set(name, static_cast<void*>(data), sizeof(T) * size); }

		void Set(const std::string& name, const void* data, int bytes);

		template<typename T>
		bool Get(const std::string& name, T* out);

		template<typename T>
		bool GetPtr(const std::string& name, T** out);

		void GetData(void** ptr, int* size) const;

	protected:
		virtual void Init() = 0;

		void AppendFieldAligned(const std::string& name, int bytes);

	private:
		struct BATCH {
			int32_t data[4];
		};
		std::unordered_map<std::string, int> fieldOffsets;
		std::vector<BATCH> buffer;
		int bufferBytes = 0;
	};
};

template<typename T>
inline bool ConstantBuffers::CBuf::Get(const std::string& name, T* out)
{
	auto iter = fieldOffsets.find(name);
	if (iter == fieldOffsets.end()) {
		return false;
	}
	const int byteOffset = iter->second;
	void* target = reinterpret_cast<char*>(buffer.data()) + byteOffset;
	*out = *static_cast<T*>(target);
	return true;
}

template<typename T>
inline bool ConstantBuffers::CBuf::GetPtr(const std::string& name, T** out)
{
	auto iter = fieldOffsets.find(name);
	if (iter == fieldOffsets.end()) {
		return false;
	}
	const int byteOffset = iter->second;
	void* target = reinterpret_cast<char*>(buffer.data()) + byteOffset;
	*out = static_cast<T*>(target);
	return true;
}
