
#ifndef READONLY_CLAU_PARSER_FOR_LARGE_H
#define READONLY_CLAU_PARSER_FOR_LARGE_H

#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <string>
#include <list>

#include <fstream>

#include <algorithm>

#include <thread>

namespace wiz {
	template <typename T>
	inline T pos_1(const T x, const int base = 10)
	{
		if (x >= 0) { return x % base; }// x - ( x / 10 ) * 10; }
		else { return (x / base) * base - x; }
		// -( x - ( (x/10) * 10 ) )
	}


	template <typename T> /// T <- char, int, long, long long...
	std::string toStr(const T x) /// chk!!
	{
		const int base = 10;
		if (base < 2 || base > 16) { return "base is not valid"; }
		T i = x;

		const int INT_SIZE = sizeof(T) << 3; ///*8
		char* temp = new char[INT_SIZE + 1 + 1]; /// 1 NULL, 1 minus
		std::string tempString;
		int k;
		bool isMinus = (i < 0);
		temp[INT_SIZE + 1] = '\0'; //

		for (k = INT_SIZE; k >= 1; k--) {
			T val = pos_1<T>(i, base); /// 0 ~ base-1
									   /// number to ['0'~'9'] or ['A'~'F']
			if (val < 10) { temp[k] = val + '0'; }
			else { temp[k] = val - 10 + 'A'; }

			i /= base;

			if (0 == i) { // 
				k--;
				break;
			}
		}

		if (isMinus) {
			temp[k] = '-';
			tempString = std::string(temp + k);//
		}
		else {
			tempString = std::string(temp + k + 1); //
		}
		delete[] temp;

		return tempString;
	}


	class LoadDataOption
	{
	public:
		char LineComment = '#';	// # 
		char Left = '{', Right = '}';	// { }
		char Assignment = '=';	// = 
		char Removal = ' ';		// ',' 
	};

	inline bool isWhitespace(const char ch)
	{
		switch (ch)
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		case '\v':
		case '\f':
			return true;
			break;
		}
		return false;
	}


	inline int Equal(const long long x, const long long y)
	{
		if (x == y) {
			return 0;
		}
		return -1;
	}

	class InFileReserver
	{
	private:
		// todo - rename.
		static long long Get(long long position, long long length, char ch) {
			long long x = (position << 32) + (length << 2) + 0;

			if (length != 1) {
				return x;
			}

			switch (ch) {
			case '{':
				x += 1;
				break;
			case '}':
				x += 2;
				break;
			case '=':
				x += 3;
				break;
			}

			return x;
		}

		static long long GetIdx(long long x) {
			return (x >> 32) & 0x00000000FFFFFFFF;
		}
		static long long GetLength(long long x) {
			return (x & 0x00000000FFFFFFFC) >> 2;
		}
		static long long GetType(long long x) { //to enum or enum class?
			return x & 3;
		}

		static void _Scanning(char* text, long long num, const long long length,
			long long*& token_arr, long long& _token_arr_size, long long*& token_arr2, long long& _token_arr_size2, const LoadDataOption& option) {


			long long token_arr_size = 0;
			long long token_arr_size2 = 0;

			{
				int state = 0;

				long long token_first = 0;
				long long token_last = -1;

				long long token_arr_count = 0;
				long long token_arr_count2 = 0;

				for (long long i = 0; i < length; ++i) {
					const char ch = text[i];

					if ('\"' == ch) {
						{//
							token_arr2[num + token_arr_count2] = Get(i + num, 1, ch);
							token_arr_count2++;
						}

						token_last = i - 1;
						if (token_last - token_first + 1 > 0) {
							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first]);
							token_arr_count++;
						}

						token_first = i;
						token_last = i;

						token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first]);
						token_arr_count++;

						token_first = i + 1;
						token_last = i + 1;
					}
					else if ('\\' == ch) {
						{//
							token_arr2[num + token_arr_count2] = Get(i + num, 1, ch);
							token_arr_count2++;
						}
					}
					else if ('\n' == ch) {
						{//
							token_arr2[num + token_arr_count2] = Get(i + num, 1, ch);
							token_arr_count2++;
						}

						token_last = i - 1;
						if (token_last - token_first + 1 > 0) {
							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first]);
							token_arr_count++;
						}
						token_first = i + 1;
						token_last = i + 1;
					}
					else if ('\0' == ch) {
						{//
							token_arr2[num + token_arr_count2] = Get(i + num, 1, ch);
							token_arr_count2++;
						}

						token_last = i - 1;
						if (token_last - token_first + 1 > 0) {
							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first]);
							token_arr_count++;
						}
						token_first = i + 1;
						token_last = i + 1;
					}
					else if (option.LineComment == ch) {
						{//
							token_arr2[num + token_arr_count2] = Get(i + num, 1, ch);
							token_arr_count2++;
						}
					}
					else if (isWhitespace(ch)) {
						token_last = i - 1;
						if (token_last - token_first + 1 > 0) {
							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first]);
							token_arr_count++;
						}
						token_first = i + 1;
						token_last = i + 1;
					}
					else if (option.Left == ch) {
						token_last = i - 1;
						if (token_last - token_first + 1 > 0) {
							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first]);
							token_arr_count++;
						}

						token_first = i;
						token_last = i;

						token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first]);
						token_arr_count++;

						token_first = i + 1;
						token_last = i + 1;
					}
					else if (option.Right == ch) {
						token_last = i - 1;
						if (token_last - token_first + 1 > 0) {
							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first]);
							token_arr_count++;
						}
						token_first = i;
						token_last = i;

						token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first]);
						token_arr_count++;

						token_first = i + 1;
						token_last = i + 1;

					}
					else if (option.Assignment == ch) {
						token_last = i - 1;
						if (token_last - token_first + 1 > 0) {
							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first]);
							token_arr_count++;
						}
						token_first = i;
						token_last = i;

						token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first]);
						token_arr_count++;

						token_first = i + 1;
						token_last = i + 1;
					}
				}

				if (length - 1 - token_first + 1 > 0) {
					token_arr[num + token_arr_count] = Get(token_first + num, length - 1 - token_first + 1, text[token_first]);
					token_arr_count++;
				}
				token_arr_size = token_arr_count;
				token_arr_size2 = token_arr_count2;
			}

			{
				//_token_arr = token_arr;
				_token_arr_size = token_arr_size;
				//_token_arr2 = token_arr2;
				_token_arr_size2 = token_arr_size2;
			}
		}

		static void ScanningNew(char* text, const long long length,
			long long*& _token_arr, long long& _token_arr_size, const LoadDataOption& option)
		{
			const int thr_num = 8;
			std::thread thr[thr_num];
			long long start[thr_num];
			long long last[thr_num];

			{
				start[0] = 0;

				for (int i = 1; i < thr_num; ++i) {
					start[i] = length / thr_num * i;

					for (long long x = start[i]; x <= length; ++x) {
						if (isWhitespace(text[x]) || '\0' == text[x] ||
							option.Left == text[x] || option.Right == text[x] || option.Assignment == text[x]) {
							start[i] = x;
							break;
						}
					}
				}
				for (int i = 0; i < thr_num - 1; ++i) {
					last[i] = start[i + 1];
					for (long long x = last[i]; x <= length; ++x) {
						if (isWhitespace(text[x]) || '\0' == text[x] ||
							option.Left == text[x] || option.Right == text[x] || option.Assignment == text[x]) {
							last[i] = x;
							break;
						}
					}
				}
				last[thr_num - 1] = length + 1;
			}
			long long real_token_arr_count = 0;

			long long* tokens = new long long[length + 1];
			long long token_count = 0;
			long long* tokens2 = new long long[length + 1];
			long long token_count2 = 0;

			long long token_arr_size[thr_num];
			long long token_arr_size2[thr_num];

			for (int i = 0; i < thr_num; ++i) {
				thr[i] = std::thread(_Scanning, text + start[i], start[i], last[i] - start[i], std::ref(tokens), std::ref(token_arr_size[i]),
					std::ref(tokens2), std::ref(token_arr_size2[i]), option);
			}

			for (int i = 0; i < thr_num; ++i) {
				thr[i].join();
			}

			{
				long long _count = 0;
				for (int i = 0; i < thr_num; ++i) {
					for (long long j = 0; j < token_arr_size[i]; ++j) {
						tokens[token_count] = tokens[start[i] + j];
						token_count++;
					}
				}
			}
			{
				for (int i = 0; i < thr_num; ++i) {
					for (long long j = 0; j < token_arr_size2[i]; ++j) {
						tokens2[token_count2] = tokens2[start[i] + j];
						token_count2++;
					}
				}
			}

			int state = 0;
			long long qouted_start;
			long long slush_start;
			bool qouted_end = false;

			long long j = 0;
			for (long long i = 0; i < token_count2; ++i) {
				const long long len = GetLength(tokens2[i]);
				const char ch = text[GetIdx(tokens2[i])];
				const long long idx = GetIdx(tokens2[i]);

				if (len > 1) {
					continue;
				}

				//
				for (; j < token_count; ++j) {
					const long long _idx = GetIdx(tokens[j]);

					if (_idx >= idx) {
						break;
					}
					else if (state == 0) {
						if (qouted_end) {
							qouted_end = false;
							continue;
						}
						tokens[real_token_arr_count] = tokens[j];
						real_token_arr_count++;
					}
				}

				if (0 == state && '\"' == ch) {
					state = 1;
					qouted_start = i;
				}
				else if (0 == state && '#' == ch) {
					state = 2;
				}
				else if (1 == state && '\\' == ch) {
					state = 3;
					slush_start = idx;
				}
				else if (1 == state && '\"' == ch) {
					state = 0;

					{
						long long idx = GetIdx(tokens2[qouted_start]);
						long long len = GetLength(tokens2[qouted_start]);
						long long type = GetType(tokens2[qouted_start]);

						len = GetIdx(tokens2[i]) - idx + 1;

						tokens[real_token_arr_count] = Get(idx, len, type);
						real_token_arr_count++;

						qouted_end = true;
					}
				}
				else if (3 == state) {
					if (idx != slush_start + 1) {
						--i;
					}
					state = 1;
				}
				else if (2 == state && ('\n' == ch || '\0' == ch)) {
					state = 0;
				}
			}

			{
				//delete[] tokens;
				delete[] tokens2;

				_token_arr = tokens;
				_token_arr_size = real_token_arr_count;
			}
		}


		static void Scanning(char* text, const long long length,
			long long*& _token_arr, long long& _token_arr_size, const LoadDataOption& option) {

			long long* token_arr = new long long[length + 1];
			long long token_arr_size = 0;

			{
				int state = 0;

				long long token_first = 0;
				long long token_last = -1;

				long long token_arr_count = 0;

				for (long long i = 0; i <= length; ++i) {
					const char ch = text[i];

					if (0 == state) {
						if (option.LineComment == ch) {
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first]);
								token_arr_count++;
							}

							state = 3;
						}
						else if ('\"' == ch) {
							state = 1;
						}
						else if (isWhitespace(ch) || '\0' == ch) {
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first]);
								token_arr_count++;
							}
							token_first = i + 1;
							token_last = i + 1;
						}
						else if (option.Left == ch) {
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first]);
								token_arr_count++;
							}

							token_first = i;
							token_last = i;

							token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first]);
							token_arr_count++;

							token_first = i + 1;
							token_last = i + 1;
						}
						else if (option.Right == ch) {
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first]);
								token_arr_count++;
							}
							token_first = i;
							token_last = i;

							token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first]);
							token_arr_count++;

							token_first = i + 1;
							token_last = i + 1;

						}
						else if (option.Assignment == ch) {
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first]);
								token_arr_count++;
							}
							token_first = i;
							token_last = i;

							token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first]);
							token_arr_count++;

							token_first = i + 1;
							token_last = i + 1;
						}
					}
					else if (1 == state) {
						if ('\\' == ch) {
							state = 2;
						}
						else if ('\"' == ch) {
							state = 0;
						}
					}
					else if (2 == state) {
						state = 1;
					}
					else if (3 == state) {
						if ('\n' == ch || '\0' == ch) {
							state = 0;

							token_first = i + 1;
							token_last = i + 1;
						}
					}
				}

				token_arr_size = token_arr_count;

				if (0 != state) {
					std::cout << "[" << state << "] state is not zero.\n";
				}
			}

			{
				_token_arr = token_arr;
				_token_arr_size = token_arr_size;
			}
		}


		static std::pair<bool, int> Scan(std::ifstream& inFile, const int num, const wiz::LoadDataOption& option, int thr_num,
			char*& _buffer, long long* _buffer_len, long long*& _token_arr, long long* _token_arr_len)
		{
			if (inFile.eof()) {
				return { false, 0 };
			}

			long long* arr_count = nullptr; //
			long long arr_count_size = 0;

			std::string temp;
			char* buffer = nullptr;
			long long file_length;

			{
				inFile.seekg(0, inFile.end);
				unsigned long long length = inFile.tellg();
				inFile.seekg(0, inFile.beg);

				file_length = length;
				buffer = new char[file_length + 1]; // 

				// read data as a block:
				inFile.read(buffer, file_length);

				buffer[file_length] = '\0';

				{
					//int a = clock();
					long long* token_arr;
					long long token_arr_size;

					ScanningNew(buffer, file_length, token_arr, token_arr_size, option);
					//int b = clock();
				//	std::cout << b - a << "ms\n";
					_buffer = buffer;
					_token_arr = token_arr;
					*_token_arr_len = token_arr_size;
					*_buffer_len = file_length;
				}
			}

			return{ true, 1 };
		}

	private:
		std::ifstream* pInFile;
	public:
		int Num;
	public:
		explicit InFileReserver(std::ifstream& inFile)
		{
			pInFile = &inFile;
			Num = 1;
		}
		bool end()const { return pInFile->eof(); } //
	public:
		bool operator() (const wiz::LoadDataOption& option, int thr_num, char*& buffer, long long* buffer_len, long long*& token_arr, long long* token_arr_len)
		{
			bool x = Scan(*pInFile, Num, option, thr_num, buffer, buffer_len, token_arr, token_arr_len).second > 0;
			return x;
		}
	};

	class Utility {
	public:
		static long long GetIdx(long long x) {
			return (x >> 32) & 0x00000000FFFFFFFF;
		}
		static long long GetLength(long long x) {
			return (x & 0x00000000FFFFFFFC) >> 2;
		}
		static long long GetType(long long x) {
			return x & 3;
		}
	};

	class Node;
	class MemoryPool;

	class Pointer {
	public:
		bool isNULL = true;
		long long node_idx = -1;
		long long pool_idx = -1;
		MemoryPool* pool = nullptr;
	public:
		explicit Pointer() { }

		const Node* operator->() const;
		
		Node* operator->();

		explicit operator bool() {
			return !isNULL;
		}

		bool operator!=(const Pointer& other) const;
		bool operator==(const Pointer& other) const;
	};

	class MemoryPool
	{
	public:
		long long root = 0;
		int pool_id = -1;
		Node* arr = nullptr;
		//	Node static_else_list[10]; // todo - real depth in real data? < 10 ?
		std::vector<Node> else_list;
		long long count = 0;
		long long size = 0;
	public:
		explicit MemoryPool() {
			else_list.reserve(10);
		}
	public:
		Pointer Get();
		Node* Convert(Pointer pointer);
	public:
		virtual ~MemoryPool(); //

		void Clear();

		const Node* operator[](const long long idx) const;

		Node* operator[](const  long long idx);
	};



	class Node {
	public:
		bool operator==(const Node& other) {
			return type = other.type && name == other.name && value == other.value;
		}
	public:
		long type = 2; // 1 itemtype, 2 usertype, -1 virtual node and usertype.
		long long name = 0;
		long long value = 0;
	public:
		Pointer This;
		Pointer first;
		Pointer last; // first->last
		Pointer parent; // first->parent
		Pointer child;
		Pointer next;
	public:
		explicit Node() {
			//
		}
	public:
		Pointer GetParent() {
			return first->parent;
		}

		// usertype
		void AddItem(long long var, long long val, MemoryPool& pool) {
			if (type != 2 && type != -1) {
				return;
			}


			if (!this->child) {
				this->child = MakeNode(pool);
				this->child->first = this->child;
				this->child->last = this->child;
				this->child->parent = this->This;
				this->child->type = 1;
				this->child->name = var;
				this->child->value = val;
			}
			else {
				Pointer temp = MakeNode(pool);

				this->child->first->last->next = temp;
				this->child->first->last = temp;
				temp->first = this->child->first;
				temp->type = 1;
				temp->name = var;
				temp->value = val;
			}
		}

		Pointer AddUserTypeItem(MemoryPool& pool, long long var = 0) {
			if (type != 2 && type != -1) {
				return Pointer();
			}

			if (!this->child) {
				this->child = MakeNode(pool);
				this->child->first = this->child;
				this->child->last = this->child;
				this->child->parent = this->This;
				this->child->type = 2;
				this->child->name = var;

				return this->child;
			}
			else {
				Pointer temp = MakeNode(pool);

				this->child->first->last->next = temp;
				this->child->first->last = temp;
				temp->first = this->child->first;
				temp->type = 2;
				temp->name = var;

				return temp;
			}
		}

		Pointer AddVirtualNode(MemoryPool& pool) {
			Pointer temp = AddUserTypeItem(pool);
			if (temp) {
				temp->type = -1;
			}

			return temp;
		}

		// itemtype

	public:

		static Pointer MakeNode(MemoryPool& pool) {
			Pointer x = pool.Get(); // using memory pool.
			x->This = x;
			x->first = x;
			x->last = x;
			
			return x;
		}

		void Save(std::ostream& stream, char* buffer) const {
			Save(stream, buffer, this->This);
		}
	private:
		void Save(std::ostream& stream, char* buffer, Pointer node) const {
			do {
				if (!node) {
					return;
				}

				if (Utility::GetLength(node->name) != 0) {
					stream << std::string(Utility::GetIdx(node->name) + buffer, 
						Utility::GetLength(node->name)) << " = ";
				}
				if (node->type == -1) {
					stream << "# = { \n";
				}
				else if (node->type == 2 && node != this->This) {
					stream << " { \n";
				}
				else if (node->type == 1) {
					stream << std::string(Utility::GetIdx(node->value) + buffer, 
						Utility::GetLength(node->value)) << " ";
				}

				if (node->type == -1 || node->type == 2) {
					Save(stream, buffer, node->child);
					if (node != this->This) {
						stream << " } \n ";
					}
				}
				node = node->next;
			} while (node);
		}

	public:
		void Link(Pointer x) {
			// this - usertype,  x - node..
			if (this->child) {
				this->child->last->next = x;
				this->child->last = x->first->last;
				x->parent = Pointer();
			}
			else {
				this->child = x;
				this->child->first = x;
				x->parent = this->This;
				x->last = x->first->last;
			}

			for (Pointer iter = x; !iter.isNULL; iter = iter->next) {
				iter->first = this->child;
			}
		}
	};



	// LoadData
	class LoadData
	{
	private:
		static long long check_syntax_error1(long long str) {
			long long len = Utility::GetLength(str);
			long long type = Utility::GetType(str); // left right assign

			if (1 == len && (type == 1 || type == 2 ||
				type == 3)) {
				throw "check syntax error 1 : " + str;
			}
			return str;
		}
		
		static int Merge(Pointer next, Pointer ut, Pointer* ut_next)
		{
			//check!!
			while (ut->child
				&& ut->child->type == -1)
			{
				ut = ut->child;
			}
			int depth = 0;

			while (true) {
				Pointer _ut = ut;
				Pointer _next = next;

				if (_ut == *ut_next) {
					*ut_next = _next;
				}

				{
					Pointer x = _ut->child;
					if (x) {
						if (x->type == -1) {
							Pointer temp = x->next;
							Pointer last = x->last;
							// delete x; //
							x = temp;

							if (x) {
								x->last = last;
							}
						}
						if (x) {
							x->first = x;
							_next->Link(x);
							_ut->child = Pointer();
						}
					}
				}

				ut = ut->GetParent();
				next = next->GetParent();

				if (next && ut) {
					//
				}
				else {
					// right_depth > left_depth
					if (!next && ut) {
						return -1;
					}
					else if (next && !ut) {
						return 1;
					}

					return 0;
				}
			}
		}

		// todo - rename.
		static int FalseMerge(Pointer next, Pointer ut, Pointer* newNode, MemoryPool& utPool) {
			//check!!
			while (ut->child
				&& ut->child->type == -1)
			{
				ut = ut->child;
			}

			while (true) {
				Pointer _ut = ut;
				Pointer _next = next;

				{
					Pointer x = _ut->child;
					if (x) {
						if (x->type == -1) {
							x->name = _next->name;
						}
					}
				}

				Pointer ut_temp = ut;
				*newNode = ut;

				ut = ut->GetParent();
				next = next->GetParent();


				if (next && ut) {
					//
				}
				else {
					// right_depth > left_depth
					if (!next && ut) {
						return -1;
					}
					else if (next && !ut) {
						Pointer temp = Node::MakeNode(utPool);
						
						temp->Link(ut_temp);

						ut = temp;
					}
					else {
						return 0;
					}
				}
			}
		}
		// todo - return  bool-type?
		static void SaveToFile(const std::string& fileName, const wiz::MemoryPool& pool, long long root) {
			std::ofstream outFile;
			outFile.open(fileName, std::ios::binary);
			if (outFile) {
				long long num = pool.count;

				outFile.write((const char*)&num, sizeof(num));
				outFile.write((const char*)&root, sizeof(root));
				outFile.write((const char*)pool.arr, sizeof(Node) * num);

				outFile.close();
			}
		}
		static void LoadFromFile(const std::string& fileName, wiz::MemoryPool& pool) {
			std::ifstream inFile;
			inFile.open(fileName, std::ios::binary);

			if (inFile) {
				long long count = 0;
				long long root;
				inFile.read((char*)&count, sizeof(count));
				inFile.read((char*)&root, sizeof(root));

				if (count > pool.size) {
					pool.Clear(); pool.else_list.clear();
					
					pool.arr = new Node[count];
					pool.count = count;
					pool.size = count;

					inFile.read((char*)pool.arr, sizeof(Node) * count);

					pool.root = root;
				}
				else {
					pool.else_list.clear();
					
					inFile.read((char*)pool.arr, sizeof(Node) * count);

					pool.root = root;
				}

				inFile.close();
			}
		}
	private:
		static bool __LoadData(const char* buffer, const long long* token_arr, long long token_arr_len, Pointer* _global, const wiz::LoadDataOption* _option,
			int start_state, int last_state, Pointer* next, MemoryPool* _pool) // first, strVec.empty() must be true!!
		{
			{
				long long count_left = 0;
				long long count_right = 0;
				long long count_eq = 0;
				long long count_other = 0; // count_other

				for (long long x = 0; x < token_arr_len; ++x) {
					switch (Utility::GetType(token_arr[x]))
					{
					case 0:
						count_other++;
						break;
					case 1:
						count_left++;
						break;
					case 2:
						count_right++;
						break;
					case 3:
						count_eq++;
						break;
					}
				}

				// chk count_ - count_eq + count_left < 0 ? -
				// count_ = count_other..
				long long chkNum = count_other - count_eq + count_left;

				if (count_right > count_left) { // maybe has virtual node? - support some virutal node
					// cf) }}} {{{   right == left but, # of virtual node is 3.
					chkNum += count_right - count_left;
				}

				if (chkNum < 0) {
					_pool->arr = nullptr;
					_pool->count = 0;
					_pool->size = 0;
				}
				else {
					_pool->arr = new Node[1 + chkNum];
					_pool->count = 0;
					_pool->size = 1 + chkNum;
				}
			}

			MemoryPool& pool = *_pool;

			std::vector<long long> varVec;
			std::vector<long long> valVec;


			if (token_arr_len <= 0) {
				return false;
			}

			const wiz::LoadDataOption& option = *_option;

			int state = start_state;
			int braceNum = 0;
			std::vector< Pointer > nestedUT(1);
			long long var = 0, val = 0;

			nestedUT.reserve(10);
			
			*_global = Node::MakeNode(pool);
			nestedUT[0] = *_global;


			long long count = 0;
			const long long* x = token_arr;
			const long long* x_next = x;

			for (long long i = 0; i < token_arr_len; ++i) {
				x = x_next;
				{
					x_next = x + 1;
				}
				if (count > 0) {
					count--;
					continue;
				}
				long long len = Utility::GetLength(token_arr[i]);

				switch (state)
				{
				case 0:
				{
					// Left 1
					if (len == 1 && (-1 != Equal(1, Utility::GetType(token_arr[i])) || -1 != Equal(1, Utility::GetType(token_arr[i])))) {
						if (!varVec.empty()) {
							for (long long x = 0; x < varVec.size(); ++x) {
								nestedUT[braceNum]->AddItem(varVec[x], valVec[x], pool);
							}

							varVec.clear();
							valVec.clear();
						}

						Pointer pTemp;

						{
							pTemp = nestedUT[braceNum]->AddUserTypeItem(pool);
						}


						braceNum++;

						/// new nestedUT
						if (nestedUT.size() == braceNum) { /// changed 2014.01.23..
							nestedUT.push_back(Pointer());
						}

						/// initial new nestedUT.
						nestedUT[braceNum] = pTemp;
						///

						state = 0;
					}
					// Right 2
					else if (len == 1 && (-1 != Equal(2, Utility::GetType(token_arr[i])) || -1 != Equal(2, Utility::GetType(token_arr[i])))) {
						state = 0;

						if (!varVec.empty()) {
							for (long long x = 0; x < varVec.size(); ++x) {
								nestedUT[braceNum]->AddItem(varVec[x], valVec[x], pool);
							}

							varVec.clear();
							valVec.clear();
						}

						if (braceNum == 0) {
							Node ut;
							Pointer temp = ut.AddVirtualNode(pool);

							temp->child = nestedUT[0]->child;
							temp->parent = nestedUT[0];
							nestedUT[0]->child->first->parent = temp;
							nestedUT[0]->child = temp;

							braceNum++;
						}

						{
							if (braceNum < nestedUT.size()) {
								nestedUT[braceNum] = Pointer();
							}
							braceNum--;
						}
					}
					else {
						if (x < token_arr + token_arr_len - 1) {
							long long _len = Utility::GetLength(token_arr[i + 1]);
							// EQ 3
							if (_len == 1 && -1 != Equal(3, Utility::GetType(token_arr[i + 1]))) {
								var = token_arr[i];

								state = 1;

								{
									count = 1;
								}
							}
							else {
								// var1
								if (x <= token_arr + token_arr_len - 1) {

									val = token_arr[i];

									varVec.push_back(check_syntax_error1(var));
									valVec.push_back(check_syntax_error1(val));

									val = 0;

									state = 0;

								}
							}
						}
						else
						{
							// var1
							if (x <= token_arr + token_arr_len - 1)
							{
								val = token_arr[i];
								varVec.push_back(check_syntax_error1(var));
								valVec.push_back(check_syntax_error1(val));
								val = 0;

								state = 0;
							}
						}
					}
				}
				break;
				case 1:
				{
					// LEFT 1
					if (len == 1 && (-1 != Equal(1, Utility::GetType(token_arr[i])) || -1 != Equal(1, Utility::GetType(token_arr[i])))) {
						for (long long x = 0; x < varVec.size(); ++x) {
							nestedUT[braceNum]->AddItem(varVec[x], valVec[x], pool);
						}


						varVec.clear();
						valVec.clear();

						///
						{
							Pointer pTemp;

							pTemp = nestedUT[braceNum]->AddUserTypeItem(pool, var);



							var = 0;
							braceNum++;

							/// new nestedUT
							if (nestedUT.size() == braceNum) {
								nestedUT.push_back(Pointer());
							}

							/// initial new nestedUT.
							nestedUT[braceNum] = pTemp;
						}
						///
						state = 0;
					}
					else {
						if (x <= token_arr + token_arr_len - 1) {
							val = token_arr[i];

							varVec.push_back(check_syntax_error1(var));
							valVec.push_back(check_syntax_error1(val));
							var = 0; val = 0;

							state = 0;
						}
					}
				}
				break;
				default:
					// syntax err!!
					throw "syntax error ";
					break;
				}
			}

			if (varVec.empty() == false) {
				for (long long x = 0; x < varVec.size(); ++x) {
					nestedUT[braceNum]->AddItem(varVec[x], valVec[x], pool);
				}

				varVec.clear();
				valVec.clear();
			}

			if (next) {
				*next = nestedUT[braceNum];
			}

			if (state != last_state) {
				throw std::string("error final state is not last_state!  : ") + toStr(state);
			}
			if (x > token_arr + token_arr_len) {
				throw std::string("error x > buffer + buffer_len: ");
			}

			return true;
		}

		static long long FindDivisionPlace(const char* buffer, const long long* token_arr, long long start, long long last, const wiz::LoadDataOption& option)
		{
			for (long long a = last; a >= start; --a) {
				long long len = Utility::GetLength(token_arr[a]);
				long long val = Utility::GetType(token_arr[a]);


				if (len == 1 && (-1 != Equal(2, val) || -1 != Equal(2, val))) { // right
					return a;
				}

				bool pass = false;
				if (len == 1 && (-1 != Equal(1, val) || -1 != Equal(1, val))) { // left
					return a;
				}
				else if (len == 1 && -1 != Equal(3, val)) { // assignment
					//
					pass = true;
				}

				if (a < last && pass == false) {
					long long len = Utility::GetLength(token_arr[a + 1]);
					long long val = Utility::GetType(token_arr[a + 1]);

					if (!(len == 1 && -1 != Equal(3, val))) // assignment
					{ // NOT
						return a;
					}
				}
			}
			return -1;
		}

		static bool _LoadData(InFileReserver& reserver, Pointer* global, const wiz::LoadDataOption option, char** _buffer, std::vector<wiz::MemoryPool*>*& _pool, const int parse_num) // first, strVec.empty() must be true!!
		{
			const int pivot_num = parse_num - 1;
			char* buffer = nullptr;
			long long* token_arr = nullptr;
			long long buffer_total_len;
			long long token_arr_len = 0;

			{
				bool success = reserver(option, 0, buffer, &buffer_total_len, token_arr, &token_arr_len);

				if (!success) {
					return false;
				}
				if (token_arr_len <= 0) {
					return true;
				}
			}


			Pointer before_next = Pointer();
			Pointer _global;

			bool first = true;
			long long sum = 0;

			long long try_num = 1;

			for(long long L=1; L <= try_num; ++L) {
				std::set<long long> _pivots;
				std::vector<long long> pivots;

				const long long num_end  = L == try_num ? token_arr_len : token_arr_len / try_num * (L); //
				const long long num_start = token_arr_len / try_num * (L - 1);

				if (pivot_num > 0) {
					std::vector<long long> pivot;
					pivots.reserve(pivot_num);
					pivot.reserve(pivot_num);

					for (int i = 0; i < pivot_num; ++i) {
						pivot.push_back(FindDivisionPlace(buffer, token_arr, num_start + ((num_end - num_start) / (pivot_num + 1)) * (i),
							num_start + ((num_end - num_start) / (pivot_num + 1)) * (i + 1) - 1, option));
					}

					for (int i = 0; i < pivot.size(); ++i) {
						if (pivot[i] != -1) {
							_pivots.insert(pivot[i]);
						}
					}

					for (auto& x : _pivots) {
						pivots.push_back(x);
					}
				}

				std::vector<Pointer> next(pivots.size() + 1, Pointer());

				{
					std::vector<MemoryPool*>* pool = new std::vector<MemoryPool*>(pivots.size() + 1);

					for (int i = 0; i < pool->size(); ++i) {
						(*pool)[i] = new MemoryPool;
						(*pool)[i]->pool_id = i;
					}

					std::vector<Pointer> __global(pivots.size() + 1);
					
				
					std::vector<std::thread> thr(pivots.size() + 1);

					{
						long long idx = pivots.empty() ? num_end : pivots[0];
						long long _token_arr_len = idx - 0 + 1;


						thr[0] = std::thread(__LoadData, buffer, token_arr, _token_arr_len, &__global[0], &option, 0, 0, &next[0], (*pool)[0]);
					}

					for (int i = 1; i < pivots.size(); ++i) {
						long long _token_arr_len = pivots[i] - (pivots[i - 1] + 1) + 1;

						thr[i] = std::thread(__LoadData, buffer, token_arr + pivots[i - 1] + 1, _token_arr_len, &__global[i], &option, 0, 0, &next[i], (*pool)[i]);

					}

					if (pivots.size() >= 1) {
						long long _token_arr_len = num_end - 1 - (pivots.back() + 1) + 1;

						thr[pivots.size()] = std::thread(__LoadData, buffer, token_arr + pivots.back() + 1, _token_arr_len, &__global[pivots.size()],
							&option, 0, 0, &next[pivots.size()], (*pool)[pivots.size()]);
					}

					// wait
					for (int i = 0; i < thr.size(); ++i) {
						thr[i].join();
					}

					// Merge
					try { // chk empty global?
						/*if (__global[0]->child && __global[0]->child->type == -1) {
							std::cout << "not valid file1\n";
							throw 1;
						}
						if (next.back()->GetParent() && next.back()->GetParent()->GetParent().isNULL == false) {
							std::cout << "not valid file2\n";
							throw 2;
						}*/

						/*
						SaveWizDB(__global[0], buffer, "0.txt");
						SaveWizDB(__global[1], buffer, "1.txt");
						SaveWizDB(__global[2], buffer, "2.txt");
						SaveWizDB(__global[3], buffer, "3.txt");
						SaveWizDB(__global[4], buffer, "4.txt");
						SaveWizDB(__global[5], buffer, "5.txt");
						SaveWizDB(__global[6], buffer, "6.txt");
						SaveWizDB(__global[7], buffer, "7.txt");
						*/
						SaveToFile(wiz::toStr(L) + "_" + wiz::toStr(0) + ".eu4", *(*pool)[0], (*pool)[0]->root);

						for (int i = 1; i < pivots.size() + 1; ++i) {
							// linearly merge and error check...
							//SaveWizDB(__global[0], buffer, "8.txt");
							//SaveWizDB(next[i-1], buffer, "9.txt");
							Pointer newNode;
							int err = //Merge(next[i - 1], __global[i], &next[i]);
								FalseMerge(next[i - 1], __global[i], &newNode, *(*pool)[i]);

							SaveToFile(wiz::toStr(L) + "_" + wiz::toStr(i) + ".eu4", 
								*(*pool)[i], newNode.node_idx);

						
							if (-1 == err) {
								std::cout << "not valid file4\n";
								throw 4;
							}	/*
							else if (i == pivots.size() && 1 == err) {
								std::cout << "not valid file5\n";
								throw 5;
							}
							*/
						}

						_global = __global[0];
					//	_pool = pool;

						for (int i = 0; i < pivots.size() + 1; ++i) {
							(*pool)[i]->Clear();
							delete (*pool)[i];
							(*pool)[i] = nullptr;
						}
					}
					catch (...) {
						delete[] token_arr;
						delete[] buffer;
						for (auto& x : *pool) {
							x->Clear();
							delete x;
						}
						buffer = nullptr;
						throw "in Merge, error";
					}

					before_next = next.back();
				}
			}

			delete[] token_arr;

			{	
				std::vector<MemoryPool*>* pool = new std::vector<MemoryPool*>();
				pool->push_back(new MemoryPool);

				LoadFromFile("1_0.eu4", *(*pool)[0]);

				_pool = pool;
				_global.pool = (*pool)[0];
				_global.isNULL = false;
				_global.node_idx = (*pool)[0]->root;
				_global.pool_idx = -1;
				_global->This.node_idx = (*pool)[0]->root;
				_global->This.pool = (*pool)[0];

				for (long long x = 0; x < (*pool)[0]->count; ++x) {
					MemoryPool* y = (*pool)[0];
					Node* z = (*y)[x];
					z->This.pool = (*pool)[0];
					z->child.pool = (*pool)[0];
					z->first.pool = (*pool)[0];
					z->last.pool = (*pool)[0];
					z->parent.pool = (*pool)[0];
					z->next.pool = (*pool)[0];
				}
			}

			*_buffer = buffer;
			*global = _global;


			return true;
		}
	public:
		static bool LoadDataFromFile(const std::string& fileName, Pointer* global, char** _buffer, std::vector<wiz::MemoryPool*>*& pool, int parse_num = 0) /// global should be empty
		{
			if (parse_num <= 0) {
				parse_num = std::thread::hardware_concurrency();
			}
			if (parse_num <= 0) {
				parse_num = 1;
			}

			bool success = true;
			std::ifstream inFile;
			inFile.open(fileName, std::ios::binary);

			if (true == inFile.fail())
			{
				inFile.close(); return false;
			}

			Pointer globalTemp;

			try {

				InFileReserver ifReserver(inFile);
				wiz::LoadDataOption option;
				option.Assignment = ('=');
				option.Left = '{';
				option.Right = '}';
				option.LineComment = ('#');
				option.Removal = ' '; // ','

				ifReserver.Num = 1 << 19;
				//	strVec.reserve(ifReserver.Num);
				// cf) empty file..
				if (false == _LoadData(ifReserver, &globalTemp, option, _buffer, pool, parse_num))
				{
					inFile.close();
					return false; // return true?
				}

				inFile.close();
			}
			catch (const char* err) {
				std::cout << err << "\n"; inFile.close();
				for (auto& x : *pool) {
					x->Clear();
					delete x;
				}
				pool->clear();
				return false;
			}
			catch (const std::string & e) {
				std::cout << e << "\n"; inFile.close();
				for (auto& x : *pool) { // if pool ?
					x->Clear();
					delete x;
				}
				pool->clear();
				return false;
			}
			catch (std::exception e) {
				std::cout << e.what() << "\n"; inFile.close();
				for (auto& x : *pool) {
					x->Clear();
					delete x;
				}
				pool->clear();
				return false;
			}
			catch (...) {
				std::cout << "not expected error" << "\n"; inFile.close();
				for (auto& x : *pool) {
					x->Clear();
					delete x;
				}
				pool->clear();
				return false;
			}


			*global = globalTemp;
			return true;
		}
		static bool LoadWizDB(Pointer* global, const std::string& fileName, char** buffer, std::vector<wiz::MemoryPool*>* pool, const int thr_num) {
			Pointer globalTemp;

			// Scan + Parse 
			if (false == LoadDataFromFile(fileName, &globalTemp, buffer, pool, thr_num)) { return false; }
			//std::cout << "LoadData End" << "\n";

			*global = globalTemp;
			return true;
		}
		// SaveQuery
		static bool SaveWizDB(const Pointer& global, char* buffer, const std::string& fileName, const bool append = false) {
			std::ofstream outFile;
			if (fileName.empty()) { return false; }
			if (false == append) {
				outFile.open(fileName);
				if (outFile.fail()) { return false; }
			}
			else {
				outFile.open(fileName, std::ios::app);
				if (outFile.fail()) { return false; }

				outFile << "\n";
			}

			/// saveFile
			global->Save(outFile, buffer); // cf) friend

			outFile.close();

			return true;
		}
	};

}


#endif

