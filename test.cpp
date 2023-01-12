#include <iostream>
#include <string>
#include <forward_list>
#include <vector>


template< class RandomIt > void bubbleSort( RandomIt first, RandomIt last )
{
	int x;
	do
	{
		x = 0;
		for (RandomIt i = first, it = first; it != last; ++i)
		{
			++it;
			if (*i > *it)
			{
				std::iter_swap(i,it);
				x++;
			}
		}
	}
	while (x != 0);
}

template<typename T> class forward_list;

template< typename Node, typename T > class forward_list_iterator;

//------------------------------------------------------------------------------

int gNodeCount = 0; // Counter for forward_list_node instances, must be zero after test()

template<typename T> class forward_list_node
{
	public:
		typedef T value_type;
		forward_list_node( const T & data, forward_list_node * next = nullptr) :data( data), next(next)
		{
			std::cout << "created node " << data << "\n";
			++gNodeCount;
		}
		
		forward_list_node( T && data, forward_list_node * next = nullptr) :data( std::move(data)), next(next)
		{
			std::cout << "moved node " << this->data << "\n";
			++gNodeCount;
		}
		
		~forward_list_node()
		{
			std::cout << "deleted node " << data << "\n";
			--gNodeCount;
		}
		
		T                   data;
		forward_list_node * next;
};


//------------------------------------------------------------------------------

template< typename Node, typename T> class forward_list_iterator
{
	public:
		friend class forward_list<typename Node::value_type>;
		friend class forward_list_iterator<Node, typename Node::value_type>;
		
		forward_list_iterator() :node_( nullptr),before_begin_( false)
		{
			
		}
		
		
		~forward_list_iterator()
		{
			
		}
		
		forward_list_iterator operator++()
		{
			increment();
			return *this;
		}
		
		forward_list_iterator operator++(int)
		{
			auto it = *this;
			increment();
			return it;
		}
		
		//convert a iterator to a const_iterator
		operator forward_list_iterator<Node, const T>()
		{
			return forward_list_iterator<Node, const T>( node_, before_begin_);
		}
		
		T & operator*() const
		{
			return node_->data;
		}
		
		T * operator->() const
		{
			return &node_->data;
		}
		
		bool operator==( const forward_list_iterator & rhs) const
		{
			return isEqual(rhs);
		}
		
		bool operator!=( const forward_list_iterator & rhs) const
		{
			return !isEqual( rhs);
		}
		
	private:
		forward_list_iterator( Node * node, bool before_begin = false) :node_( node), before_begin_( before_begin)
		{
			
		}
		
		void increment()
		{
			node_ = node_->next;
			before_begin_ = false;
		}
		
		bool isEqual( const forward_list_iterator & rhs) const
		{
			return node_ == rhs.node_ && before_begin_ == rhs.before_begin_;
		}
		
		Node * node_;
		bool   before_begin_;
};

//------------------------------------------------------------------------------

template<typename T> class forward_list
{
	public:
		typedef T                                                      value_type;
		typedef std::size_t                                            size_type;
		typedef forward_list_iterator<forward_list_node<T>, T>         iterator;
		typedef forward_list_iterator<forward_list_node<T>, const T>   const_iterator;
		
		forward_list()
		{
			
		}
		forward_list( const forward_list& other)
		{
			auto cur = before_begin();
			auto it = other.begin();
			do
			{
				cur = insert_after(cur, it.node_->data);
				if (it.node_->next == nullptr) break;
				++it;
			}
			while (1);
		}
		forward_list( forward_list&& other) :head_(other.head_)
		{
			other.head_ = nullptr;
		}
		
		~forward_list()
		{
			clear();
		}
		forward_list( std::initializer_list<T> init)
		{
			for( auto it = std::rbegin(init); it != std::rend(init); ++it)
			{
				head_ = new forward_list_node<T>( *it, head_);
			}
		}
		forward_list<T> & operator=(forward_list<T> & other)
		{
			if (*this != other)
			{
				clear();
				auto cur = before_begin();
				auto it = other.begin();
				do
				{
					cur = insert_after(cur, it.node_->data);
					if (it.node_->next == nullptr)
					{
						break;
					}
					++it;
				}
				while (1);
			}
			return *this;
		}
		iterator       begin()
		{
			return       iterator( head_);
		}
		const_iterator begin() const
		{
			return const_iterator( head_);
		}
		const_iterator cbegin()const
		{
			return const_iterator( head_);
		}
		
		iterator       before_begin()
		{
			return       iterator( head_,true);
		}
		const_iterator before_begin() const
		{
			return const_iterator( head_,true);
		}
		const_iterator cbefore_begin()const
		{
			return const_iterator( head_,true);
		}
		
		iterator       end ()
		{
			return       iterator( nullptr);
		}
		const_iterator end () const
		{
			return const_iterator( nullptr);
		}
		const_iterator cend() const
		{
			return const_iterator( nullptr);
		}
		
		bool empty() const
		{
			return !head_;
		}
		
		void clear() noexcept
		{
			forward_list_node<T> * ptr;
			for ( ptr = head_; ptr != nullptr; )
			{
				auto ptr2 = ptr->next;
				delete ptr;
				ptr = ptr2;
			}
			head_ = nullptr;
		}
		void push_front( const T& value )
		{
			head_ = new forward_list_node<T>( value, head_);
		}
		void pop_front()
		{
			auto nxt = head_->next;
			delete head_;
			head_ = nxt;
		}
		iterator insert_after( const_iterator pos, const T& value )
		{
			auto & head = pos.before_begin_ ? head_ : pos.node_->next;
			head = new forward_list_node<T>( value, head);
			return iterator(head);
		}
		iterator erase_after( const_iterator pos )
		{
			auto & head = pos.before_begin_ ? head_ : pos.node_->next;
			auto heado = head;
			head = head->next;
			delete heado;
			return iterator(head);
		}
		void reverse() noexcept
		{
			forward_list_node<T> * ptr = nullptr;
			for (auto it = begin(); it != end();)
			{
				auto it2 = iterator(it.node_->next);
				it.node_->next = ptr;
				ptr = it.node_;
				it = it2;
			}
			head_ = ptr;
		}
		void unique()
		{
			for (auto it = iterator(head_); it != end(); ++it)
			{
				auto cur = iterator(it.node_->next);
				while (cur != end() && (*cur == *it))
				{
					cur = erase_after(it);
				}
			}
		}
		void sort()
		{
			auto it = iterator(head_);
			for (; it.node_->next != nullptr; ++it);
			bubbleSort(begin(), it);
		}
		
		void merge(forward_list & other)
		{
			forward_list_node<T> ** he = &head_;
			for (auto it = iterator(head_), it2 = iterator(other.head_); !(it == nullptr && it2 == nullptr);)
			{
				if (it2 != nullptr && (it == nullptr || *it > *it2))
				{
					*he = it2.node_;
					he = &it2.node_->next;
					++it2;
				}
				else
				{
					*he = it.node_;
					he = &it.node_->next;
					++it;
				}
			}
			
			other.head_ = nullptr;
		}
		
	private:
		forward_list_node<T> * head_ = nullptr;
};
template< class T > bool operator==( const forward_list<T>& lhs, const forward_list<T>& rhs )
{
	auto it = lhs.begin(), st = rhs.begin();
	for (; it != lhs.end() && st != rhs.end(); ++it, ++st)
	{
		if (*it != *st) return false;
	}
	if (it != lhs.end() || st != rhs.end()) return false;
	return true;
}

template< class T > bool operator!=( const forward_list<T>& lhs, const forward_list<T>& rhs )
{
	return !(lhs == rhs);
}

template<typename List, typename VEC> void print( const List & l, VEC & vec, const std::string & caption)
{
	vec.push_back({caption,{}});
	std::cout << "--------- " << caption << " ---------\n";
	for( auto s: l)
	{
		std::cout << s << "\n";
		vec.back().second.push_back(s);
	}
	std::cout << "------------------\n";
}

template<typename List> auto test()
{
	std::vector<std::pair<std::string,std::vector<typename List::value_type>>> res;
	
	std::cout << "-------- test -------\n";
	List l = {1, 1, 2, 2, 3, 4 ,5 , 2 };
	
	print(l,res,"initial");
	
	l.unique();
	print(l,res,"unique");
	
	l.reverse();
	print(l,res,"reverse");
	
	l.erase_after( l.before_begin());
	l.erase_after( l.begin());
	print(l,res,"erase_after");
	
	//auto cit = l.cbegin();
	List lm;
	lm = l;
	lm = lm;
	print(lm,res,"copied");
	lm.sort();
	print(lm,res,"sorted");
	
	lm.merge(l);
	print(lm,res,"merged");
	//*it = "X";
	//	*cit = "X";
	
	//cit = it;
	//std::cout << *cit << "\n";
	//	bool same = it == cit;
	
	//bool same = it == cit;
	//std::cout << same << "\n";
	
	//l.insert_after( l.before_begin(), "The new first element");	
	//l.insert_after( l.begin(), "The new element after begin()");	
	
	//l.pop_front();
	
	//l.erase_after( l.before_begin());
	//l.erase_after( l.begin());
	
	return res;
}

template<typename VEC> void checkResults( const VEC & r1, const VEC & r2)
{
	for( auto it1 = r1.begin(), it2 = r2.begin(); it1!=r1.end() && it2!=r2.end();++it1,++it2 )
	{
		if( *it1 != *it2)
		{
			std::cout << "- " << it1->first << "\n";
			const auto & v1 = it1->second;
			const auto & v2 = it2->second;
			auto n = std::max( v1.size(), v2.size());
			for( auto i = 0; i < n; ++i)
			{
				auto ok = i < v1.size() && i < v2.size() && v1[i] == v2[i];
				
				if(i < v1.size())
				{
					std::cout << v1[i];
				}
				else 
				{
					std::cout << "-";
				}
				std::cout << "  <-> ";
				if(i < v2.size())
				{
					std::cout << v2[i];
				}
				else 
				{
					std::cout << "-";
				}
				if( !ok)
				{
					std::cout << " <---- ERROR";
				}
				std::cout << "\n";
			}
		}
	}
}

int main()
{
	auto r2 = test<forward_list<int>>();
	auto r1 = test<std::forward_list<int>>();
	
	if( r1==r2)
	{
		std::cout << "\n### test succeeded\n";
	}
	else
	{
		std::cout << "\n### tests failed:\n";
		checkResults(r1,r2);
	}
	
	if( gNodeCount != 0)
	{
		std::cout << "\n### Memory leaks\n";
		std::cout << gNodeCount << " nodes were not deleted \n";
	}
	
	return 0;
}
