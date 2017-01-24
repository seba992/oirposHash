#pragma once
#include <xstddef>



template<class key_type,
		class value_type>

class double_hash_map
{
public:


	class iterator
	{
	public:
		typedef iterator self_type;
		typedef key_type& reference;
		typedef std::pair<key_type, value_type>* pointer;
		typedef std::forward_iterator_tag iterator_category;
		iterator(pointer ptr, double_hash_map<key_type, value_type> *table) : ptr_(ptr), tmp(table) { }
		self_type operator++() { self_type i = *this; ptr_ = tmp->next(ptr_); return i; }
		self_type operator++(int junk) { ptr_++; return *this; }
		reference operator*() { return *ptr_; }
		pointer operator->() { return ptr_; }
		bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
		bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
	private:
		pointer ptr_;
		double_hash_map<key_type, value_type> *tmp;

	};

	class const_iterator
	{
	public:
		typedef const_iterator self_type;
		//typedef key_type value_type;
		typedef key_type& reference;
		typedef std::pair<key_type, value_type>* pointer;
		typedef std::forward_iterator_tag iterator_category;
		const_iterator(pointer ptr, double_hash_map<key_type, value_type> const *table) : ptr_(ptr), tmp(table) { }
		self_type operator++() { self_type i = *this; ptr_ = tmp->next(ptr_); return i; }
		self_type operator++(int junk) { ptr_++; return *this; }
		const reference operator*() { return *ptr_; }
		const pointer operator->() { return ptr_; }
		bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
		bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
	private:
		pointer ptr_;
		double_hash_map<key_type, value_type> *tmp;

	};

	iterator begin()
	{
		//preventing from returning empty value as first iterator
		for (size_t i = 0; i < _sizes[_actual_size]; i++)
		{
			if (_status_table[i] == occupied)
			{
				return iterator(_table + i, this);
			}
		}
		return end();
	}

	iterator end()
	{
		return iterator(_table + _sizes[_actual_size], this);
	}

	const_iterator cbegin() const
	{
		//preventing from returning empty value as first iterator
		for (size_t i = 0; i < _sizes[_actual_size]; i++)
		{
			if (_status_table[i] == occupied)
			{
				return const_iterator(_table + i, this);
			}
		}
		return cend();
	}

	const_iterator cend() const
	{
		return const_iterator(_table + _sizes[_actual_size], this);
	}

	double_hash_map(double_hash_map<key_type, value_type> &other)
	{
		first_hash = other.first_hash;
		second_hash = other.second_hash;
		_table = other._table;
		_status_table = other._status_table;
		_size = other._size;
		_actual_size = other._actual_size;
		_max_load_factor = other.max_load_factor;
	}

	double_hash_map()
	{
		_table = new key_type[_sizes[_actual_size]];
		_status_table = new status[_sizes[_actual_size]];
		for (size_t i = 0; i < _sizes[_actual_size]; i++)
		{
			_status_table[i] = free;

		}
	}
	double_hash_map(size_t(*fun)(key_type))
	{
		first_hash = fun;
		_table = new key_type[_sizes[_actual_size]];
		_status_table = new status[_sizes[_actual_size]];

		for (size_t i = 0; i < _sizes[_actual_size]; i++)
		{
			_status_table[i] = free;
		}
	}

	double_hash_map(size_t(*fir)(key_type), size_t(*sec)(key_type))
	{
		first_hash = fir;
		second_hash = sec;
		_table = new pair<key_type, value_type>[_sizes[_actual_size]];
		_status_table = new status[_sizes[_actual_size]];

		for (size_t i = 0; i < _sizes[_actual_size]; i++)
		{
			_status_table[i] = free;

		}
	}

	~double_hash_map()
	{
		if (_table != NULL)
		{
			_table = NULL;
			delete _table;
		}
		if (_status_table != NULL)
		{
			_status_table = NULL;
			delete _status_table;
		}
	}

	bool empty()
	{
		return !_size;
	}

	int size()
	{
		return _size;
	}

	int max_size()
	{
		//get the last element in size array
		return _sizes[(sizeof(_sizes) / sizeof(*_sizes)) - 1];
	}

	//change return key_type to iterator of inserted value
	iterator insert(std::pair<key_type, value_type> obj)
	{
		int i;
		iterator ret = end();
		for (i = 0; i<100; i++)
		{
			int pos = get_hash(obj.first, i);
			if (_status_table[pos] != occupied)
			{
				_table[pos] = obj;
				_status_table[pos] = occupied;
				_size++;
				ret = iterator(_table + pos, this);
				break;
			}
			else if (_table[pos].first == obj.first)
			{
				ret = end();
				break;
			}
		}
		if (i == 100)
			cout << "ERROR VALUE NOT INSERTED" << endl;
		if (load_factor() > max_load_factor())
		{
			rehash(_sizes[(_actual_size + 1)]);
		}
		return ret;
	}

	void emplace(std::pair<key_type, value_type> object)
	{
		insert(std::make_pair<key_type, value_type>(key_type(object.first), value_type(object.second)));
	}

	double max_load_factor()
	{
		return _max_load_factor;
	}

	void max_load_factor(double factor)
	{
		_max_load_factor = factor;
	}
	
	iterator find(key_type object)
	{

		int i = 0;
		do
		{
			int pos = get_hash(object, i);
			if (_status_table[pos] == free)
				return end();
			else if (_table[pos].first == object)
				return iterator(_table + pos, this);
			i++;
		} while (true /*i < _sizes[_actual_size]*/);
		return end();
	}

	value_type& at(key_type key)
	{
		int i = 0;
		do
		{
			int pos = get_hash(key, i);
			if (_status_table[pos] == free)
			{
				std::pair<key_type, value_type> tmp(key, value_type());
				iterator it = insert(tmp);
				return it->second;
			}
			else if (_table[pos].first == key)
				return _table[pos].second;
			i++;
		} while (true/*i < _sizes[_actual_size]*/);

	}

	value_type& operator[](key_type key)
	{
		return at(key);
	}

	int bucket(key_type key)
	{

		for (size_t i = 0; true; i++)
		{
			int pos = get_hash(key, i);
			if (_status_table[pos] == free)
				return -1;
			else if (_table[pos].first == key)
				return pos;
		}
	}

	int count(key_type key)
	{

		for (size_t i = 0; true; i++)
		{
			int pos = get_hash(key, i);
			if (_status_table[pos] == free)
				return 0;
			else if (_table[pos].first == key)
				return 1;
		}
	}

	//returns number of erased elements
	int erase(key_type key)
	{
		for (size_t i = 0; true; i++)
		{
			int pos = get_hash(key, i);
			if (_status_table[pos] == free)
				return 0;
			else if (_table[pos].first == key)
			{
				_status_table[pos] = available;
				_table[pos].first.~key_type();
				_table[pos].second.~value_type();
				_size--;
				return 1;
			}
		}
	}

	void clear()
	{
		for (size_t i = 0; i < _sizes[_actual_size]; i++)
		{
			if (_status_table[i] != occupied)
			{
				_status_table[i] = free;
			}
			else
			{
				_status_table[i] = free;
				_table[i].first.~key_type();
				_table[i].second.~value_type();
			}
		}
		_size = 0;
	}

	double_hash_map<key_type, value_type> operator=(double_hash_map<key_type, value_type> &other)
	{
		first_hash = other.first_hash;
		second_hash = other.second_hash;
		_table = other._table;
		_status_table = other._status_table;
		_size = other._size;
		_actual_size = other._actual_size;
		_max_load_factor = other._max_load_factor;
		return *this;
	}

	void swap(double_hash_map<key_type, value_type> &hash_tab)
	{
		double_hash_map<key_type> tmp;
		tmp = hash_tab;
		hash_tab = *this;
		*this = tmp;
		tmp._table = NULL;
		tmp._status_table = NULL;
	}

	int bucket_count()
	{
		return _sizes[_actual_size];
	}

	double load_factor()
	{
		return (double)size() / (double)bucket_count();
	}

	int max_bucket_count()
	{
		return max_size();
	}

	int bucket_size(int pos)
	{
		if (_status_table[pos] == occupied)
			return 1;
		else
			return 0;
	}


	void reserve(int len)
	{
		rehash(len);
	}

	void rehash(int len)
	{
		pair<key_type, value_type> *tmp;
		status * tmp_status;

		int old_size = _sizes[_actual_size];
		tmp = _table;
		tmp_status = _status_table;

		while (_sizes[_actual_size] < len)
		{
			_actual_size++;
		}
		_table = new pair<key_type, value_type>[_sizes[_actual_size]];
		_status_table = new status[_sizes[_actual_size]];
		_size = 0;
		for (size_t i = 0; i < _sizes[_actual_size]; i++)
		{
			_status_table[i] = free;

		}
		for (size_t i = 0; i < old_size; i++)
		{

			if (tmp_status[i] == occupied)
				insert(tmp[i]);
		}

		//if (tmp != NULL)
		//	delete tmp;
		//if (tmp_status != NULL)
		//	delete tmp_status;

	}
private:
	static enum status { free = 0, available, occupied };
	size_t(*first_hash)(key_type);
	size_t(*second_hash)(key_type);
	std::pair<key_type, value_type> *_table;
	status *_status_table;
	int _size = 0;
	int _actual_size = 0;
	double _max_load_factor = 0.85;
	//there are prime numbers to be set as size of table
	const int _sizes[9] = { 11, 23, 51, 163841, 545747, 1090373, 10570841, 49979687, 104395301 };

	size_t get_hash(key_type obj, int iter)
	{
		return (first_hash(obj) + iter*second_hash(obj)) % _sizes[_actual_size];
	}

	key_type* next(key_type* it)
	{
		do
		{
			if (it == _table + _sizes[_actual_size])
				return it;
			++it;
			if (it == _table + _sizes[_actual_size])
				return it;
			if (_status_table[it - _table] == occupied)
				break;
		} while (true);
		return it;
	}
};
