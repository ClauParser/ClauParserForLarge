
#include "readonly_clau_parser_for_large.h"


namespace wiz {


	const Node* Pointer::operator->() const
	{
		return (*pool)[node_idx];
	}

	Node* Pointer::operator->()
	{
		return (*pool)[node_idx] ;
	}


	const Node* MemoryPool::operator[](const  long long idx) const
	{
		if (idx < this->size) {
			return &this->arr[idx];
		}
		else {
			return &this->else_list[idx - size];
		}

	}

	bool Pointer::operator!=(const Pointer& other) const {
		return !(*this == other);
	}
	bool Pointer::operator==(const Pointer& other) const {
		if (this->isNULL && other.isNULL) {
			return true;
		}
		return this->isNULL == other.isNULL && this->node_idx == other.node_idx && this->pool == other.pool;
	}

	Node* MemoryPool::operator[](const  long long idx)
	{
		if (idx < this->size) {
			return &this->arr[idx];
		}
		else {
			return &this->else_list[idx - size];
		}
	}

	Pointer MemoryPool::Get() {
		if (size > 0 && count < size) {
			count++;
			Pointer pointer;
			pointer.node_idx = count - 1;
			pointer.pool_idx = pool_id;
			pointer.isNULL = false;
			pointer.pool = this;
			return pointer;
		}
		else {
			// in real depth?  <= 10 ?
			// static Node[10] and list<Node*> ?
			count++; // for number check.
			else_list.push_back(Node());

			Pointer pointer;
			pointer.node_idx = count - 1;
			pointer.pool_idx = pool_id;
			pointer.isNULL = false;
			pointer.pool = this;
			return pointer;
		}
	}

	Node* MemoryPool::Convert(Pointer pointer) {//
		if (pointer.pool_idx != this->pool_id) {
			return nullptr;
		}
		if (pointer.node_idx < this->size) {
			return &this->arr[pointer.node_idx];
		}
		else {
			return &this->else_list[pointer.node_idx - size];
		}
	}

	MemoryPool::~MemoryPool() {
		//Clear();
	}

	void MemoryPool::Clear() { // maybe just one called....
		if (arr) {
			delete[] arr;
			arr = nullptr;
		}
		else_list.clear();
	}
}

