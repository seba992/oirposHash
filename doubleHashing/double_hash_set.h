#pragma once
#include <xstddef>
#include <math.h>
#include <xhash>
#include <stdlib.h>
#include <memory>



#define MASK 0x03
#define SET_FREE 0x00
#define SET_REMOVED 0x01
#define SET_OCCUPIED 0x02

template<class type>
class double_hash_set
{
public:


	template<class type>
	class ptr_iterator
		: public std::iterator<std::forward_iterator_tag, type>
	{
		typedef ptr_iterator<type>  iterator;
		pointer pos_;
		double_hash_set<type> *_set;
	public:
		ptr_iterator() : pos_(nullptr) {}
		ptr_iterator(type* v, double_hash_set<type> *set) : pos_(v), _set(set) {}
		~ptr_iterator() {}

		iterator  operator++(int) { pos_ = _set->next(pos_);  return pos_; }
		iterator& operator++()   { pos_ = _set->next(pos_); return *this; }
		reference operator* () const { return *pos_; }
		pointer   operator->() const { return pos_; }
		iterator  operator+ (difference_type v)   const { for (int i = 0; i < v; i++) pos_ = _set->next(pos_); return pos_; }
		bool      operator==(const iterator& rhs) const { return pos_ == rhs.pos_; }
		bool      operator!=(const iterator& rhs) const { return pos_ != rhs.pos_; }
	};

	
	ptr_iterator<type> begin()
	{
		//preventing from returning empty value as first iterator
		for (size_t i = 0; i < _sizes[_actual_size_index]; i++)
		{
			if (get_status(i) == occupied)
			{
				return ptr_iterator<type>(_table + i, this);
			}
		}
		return end();
	}

	ptr_iterator<type> end()
	{
		return ptr_iterator<type>(_table + _sizes[_actual_size_index], this);
	}


	double_hash_set(const double_hash_set<type> &other)
	{
		first_hash = other.first_hash;
		second_hash = other.second_hash;
		_size = other._size;
		_internal_size = other._internal_size;
		_actual_size_index = other._actual_size_index;
		_max_load_factor = other._max_load_factor;

		_table =alloc.allocate(_sizes[_actual_size_index]);

		int status_size = (int)ceil(_sizes[_actual_size_index] / 4);
		_status_table = new unsigned char[status_size + 1];

		for (size_t i = 0; i < _sizes[_actual_size_index]; i++)
		{
			if(i < status_size)
				_status_table[i] = other._status_table[i];

			_table[i] = other._table[i];
		}


	}

	double_hash_set()
	{
		_table = new type[_sizes[_actual_size_index]];

		int status_size = (int)ceil(_sizes[_actual_size_index] / 4);
		_status_table = new unsigned char[status_size + 1];

		for (size_t i = 0; i < status_size; i++)
		{
			_status_table[i] = 0x00;
		}
	}

	double_hash_set(size_t(*fun)(type))
	{
		 first_hash = fun;
		_table =alloc.allocate(_sizes[_actual_size_index]);
		
		int status_size = (int)ceil(_sizes[_actual_size_index] / 4);
		_status_table = new unsigned char [status_size + 1];

		for (size_t i = 0; i < status_size; i++)
		{
			_status_table[i] = 0x00;
		}
	}

	double_hash_set(size_t(*fir)(type), size_t(*sec)(type))
	{
		first_hash = fir;
		second_hash = sec;

		_table = alloc.allocate( _sizes[_actual_size_index] + 1);
		int status_size = (int)ceil(_sizes[_actual_size_index] / 4);
		_status_table = new unsigned char [status_size + 1];

		for (size_t i = 0; i < status_size; i++)
		{
			_status_table[i] = 0x00;
		}
	}

	double_hash_set(size_t(*fir)(type), size_t(*sec)(type), std::allocator<type> All)
	{
		first_hash = fir;
		second_hash = sec;
		alloc = All;

		_table = alloc.allocate(_sizes[_actual_size_index]);

		int status_size = (int)ceil(_sizes[_actual_size_index] / 4);
		_status_table = new unsigned char[status_size + 1];

		for (size_t i = 0; i < status_size; i++)
		{
			_status_table[i] = 0x00;
		}
	}

	double_hash_set(size_t(*fir)(type), size_t(*sec)(type), std::equal_to<type> eq)
	{
		first_hash = fir;
		second_hash = sec;
		equal = eq;

		_table = alloc.allocate(_sizes[_actual_size_index]);

		int status_size = (int)ceil(_sizes[_actual_size_index] / 4);
		_status_table = new unsigned char[status_size + 1];

		for (size_t i = 0; i < status_size; i++)
		{
			_status_table[i] = 0x00;
		}
	}

	double_hash_set(size_t(*fir)(type), size_t(*sec)(type), std::equal_to<type> eq, std::allocator<type> All)
	{
		first_hash = fir;
		second_hash = sec;
		equal = eq;
		alloc = All;

		_table = alloc.allocate(_sizes[_actual_size_index]);

		int status_size = (int)ceil(_sizes[_actual_size_index] / 4);
		_status_table = new unsigned char[status_size + 1];

		for (size_t i = 0; i < status_size; i++)
		{
			_status_table[i] = 0x00;
		}
	}

	~double_hash_set()
	{
		if (_table != NULL)
		{
			alloc.deallocate(_table, _sizes[_actual_size_index]);
			_table = NULL;
			
		}
		if (_status_table != NULL)
		{
			delete[] _status_table;
			_status_table = NULL;
			
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

	ptr_iterator<type> insert(type obj)
	{

		int i;
		ptr_iterator<type> ret = end();
		size_t pos = first_hash(obj) % _sizes[_actual_size_index];


		if (get_status(pos) != occupied)
		{
			alloc.construct(_table + pos, obj);
			//_table[pos] = obj;
			set_status(pos, occupied);
			_size++;
			_internal_size++;
			ret = ptr_iterator<type>(_table + pos, this);
		}
		else
		{
			size_t s_hash = (second_hash(obj) % (_sizes[_actual_size_index] - 1)) + 1;

			for (i = 0; i<_sizes[_actual_size_index]; i++)
			{
				pos = (pos + s_hash) % _sizes[_actual_size_index];
				if (get_status(pos) != occupied)
				{
					alloc.construct(_table + pos, obj);
					//_table[pos] = obj;
					set_status(pos, occupied);
					_size++;
					_internal_size++;
					ret = ptr_iterator<type>(_table + pos, this);
					break;
				}
				else if (_table[pos] == obj)
				{
					ret = ptr_iterator<type>(_table + pos, this);
					break;
				}
			}

		}

		
		
		if (is_rehash_needed())
		{
			rehash();
		}
		return ret;
	}

	void emplace(type object)
	{
		insert(type(object));
	}

	double max_load_factor()
	{
		return _max_load_factor;
	}

	void max_load_factor(double factor)
	{
		_max_load_factor = factor;
	}

	ptr_iterator<type> find(type obj)
	{
		ptr_iterator<type> ret = end();
		size_t pos = first_hash(obj) % _sizes[_actual_size_index];


		if (get_status(pos) == free)
		{
			ret = end();
		}
		else if(get_status(pos) == occupied)
		{
			if (equal(_table[pos], obj))
				ret = ptr_iterator<type>(_table + pos, this);
		}
		else
		{
			size_t s_hash = (second_hash(obj) % (_sizes[_actual_size_index] - 1)) + 1;

			for (int i = 0; i<_sizes[_actual_size_index]; i++)
			{
				pos = (pos + s_hash) % _sizes[_actual_size_index];
				if (get_status(pos) == free)
				{
					return end();
				}
				else if (equal(_table[pos], obj))
				{
					return ptr_iterator<type>(_table + pos, this);
				}
			}
		}
		return ret;
	}

	int bucket(type object)
	{
		size_t pos = first_hash(obj) % _sizes[_actual_size_index];
		if (get_status(pos) == free)
		{
			return -1;
		}
		else if (get_status(pos) == occupied)
		{
			if (equal(_table[pos], obj))
				return pos;
		}
		else
		{
			size_t s_hash = (second_hash(obj) % (_sizes[_actual_size_index] - 1)) + 1;

			for (int i = 0; i<_sizes[_actual_size_index]; i++)
			{
				pos = (pos + s_hash) % _sizes[_actual_size_index];
				if (get_status(pos) == free)
				{
					return -1;
				}
				else if (equal(_table[pos], obj))
				{
					return pos;
				}
			}
		}
	}

	int count(type object)
	{
		size_t pos = first_hash(obj) % _sizes[_actual_size_index];
		if (get_status(pos) == free)
		{
			return 0;
		}
		else if (get_status(pos) == occupied)
		{
			if (equal(_table[pos], obj))
				return 1;
		}
		else
		{
			size_t s_hash = (second_hash(obj) % (_sizes[_actual_size_index] - 1)) + 1;

			for (int i = 0; i<_sizes[_actual_size_index]; i++)
			{
				pos = (pos + s_hash) % _sizes[_actual_size_index];
				if (get_status(pos) == free)
				{
					return 0;
				}
				else if (equal(_table[pos], obj))
				{
					return 1;
				}
			}
		}
	}

	//returns number of erased elements
	int erase(type object)
	{

		size_t pos = first_hash(obj) % _sizes[_actual_size_index];
		if (get_status(pos) == free)
		{
			return 0;
		}
		else if (get_status(pos) == occupied)
		{
			if (equal(_table[pos], obj))
			{
				get_status(pos) = removed;
				_table[pos].~type();
				_size--;
				return 1;
			}
		}
		else
		{
			size_t s_hash = (second_hash(obj) % (_sizes[_actual_size_index] - 1)) + 1;

			for (int i = 0; i<_sizes[_actual_size_index]; i++)
			{
				pos = (pos + s_hash) % _sizes[_actual_size_index];
				if (get_status(pos) == free)
				{
					return o;
				}
				else if (_equal(_table[pos], obj))
				{
					get_status(pos) = removed;
					_table[pos].~type();
					_size--;
					return 1;
				}
			}
		}
	}

	void clear()
	{
		for (size_t i = 0; i < _sizes[_actual_size_index]; i++)
		{
			if (get_status(int i) != occupied)
			{
				get_status(int i) = free;
			}
			else
			{
				get_status(int i) = free;
				_table[i].~type();
			}
		}
		_size = 0;
		_internal_size = 0;
	}


	double_hash_set<type> operator=(double_hash_set<type> &other)
	{
		first_hash = other.first_hash;
		second_hash = other.second_hash;
		_size = other._size;
		_internal_size = other._internal_size;
		_actual_size_index = other._actual_size_index;
		_max_load_factor = other._max_load_factor;

		_table =alloc.allocate(_sizes[_actual_size_index]);

		int status_size = (int)ceil(_sizes[_actual_size_index] / 4);
		_status_table = new unsigned char[status_size + 1];

		for (size_t i = 0; i < _sizes[_actual_size_index]; i++)
		{
			if (i < status_size)
				_status_table[i] = other._status_table[i];

			_table[i] = other._table[i];
		}


		return *this;
	}


	void swap(double_hash_set<type> &hash_tab)
	{
		std::swap(hash_tab.first_hash, first_hash);
		std::swap(hash_tab.second_hash, second_hash);
		std::swap(hash_tab._table, _table);
		std::swap(hash_tab._status_table, _status_table);
		std::swap(hash_tab._size, _size);
		std::swap(hash_tab._internal_size, _internal_size);
		std::swap(hash_tab._actual_size_index, _actual_size_index);
		std::swap(hash_tab._max_load_factor, _max_load_factor);
	}

	int bucket_count()
	{
		return _sizes[_actual_size_index];
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
		if (get_status(pos) == occupied)
			return 1;
		else
			return 0;
	}

	void debug()
	{
		//for (size_t i = 0; i < _sizes[_actual_size]; i++)
		//{
		//	if (_table[i] != NULL)
		//		std::cout << i << " -> " << _table[i] << "  " << get_status(int i) << std::endl;
		//	else
		//		std::cout << i << " -> NULL" << "  " << get_status(int i) << std::endl;


		//}
		set_status(5, occupied);
		cout << get_status(5) << endl;
		set_status(5, free);
		cout << get_status(5) << endl;
		set_status(5, occupied);
		cout << get_status(5) << endl;
		set_status(5, removed);
		cout << get_status(5) << endl;

	}

	void reserve(int len)
	{
		rehash(len);
	}

	void rehash(int len)
	{
		type *tmp;
		unsigned char * tmp_status;

		int old_size = _sizes[_actual_size_index];
		tmp = _table;
		tmp_status = _status_table;
		
		while (_sizes[_actual_size_index] < len)
		{
			_actual_size_index++;
		}

		_table =alloc.allocate(_sizes[_actual_size_index]);

		int status_size = (int)ceil(_sizes[_actual_size_index] / 4);
		_status_table = new unsigned char[status_size + 1];
		_size = 0;
		_internal_size = 0;
		for (size_t i = 0; i < _sizes[_actual_size_index]; i++)
		{
			
			set_status(i, free);

		}
		for (size_t i = 0; i < old_size; i++)
		{
			
			if (get_status(i, tmp_status) == occupied)
				insert(tmp[i]);
		}


		if(tmp != NULL)
			alloc.deallocate(tmp, old_size);
		if(tmp_status != NULL)
			delete[] tmp_status;
		
	}
private:
	size_t(*first_hash)(type);
	size_t(*second_hash)(type);
	std::allocator<type> alloc;
	std::equal_to<type> equal;

	static enum status {free = 0, removed = 1, occupied = 2};
	type *_table;
	unsigned char *_status_table;

	int _size = 0;
	int _internal_size = 0;
	int _actual_size_index = 0;
	double _max_load_factor = 0.85;
	//there are prime numbers to be set as size of table
	const int64_t _sizes[37] = { 19, 41, 83, 167, 337, 667, 1327, 2659, 5323, 10651,
		21139, 42281, 84589, 169181, 338369, 676747, 1353551, 2707109, 5414231,
		10828463, 21656933, 43313867, 86627741, 173255503, 346511021, 693022073,
		1386044159, 2772088339, 5544176683, 11088353477, 22176706961, 44353413947,
		88706827903, 177413655821, 354827311667, 709654623341 };

	const int64_t _max_load_factor_size[37] = { 16, 34, 70, 141, 286, 566, 1127, 2260,
		4524, 9053, 17968, 35938, 71900, 143803, 287613, 575234, 1150518, 2301042,
		4602096, 9204193, 18408393, 36816786, 73633579, 147267177, 294534367,
		589068762, 1178137535, 2356275088, 4712550180, 9425100455, 18850200916,
		37700401854, 75400803717, 150801607447, 301603214916, 603206429839 };

	size_t get_hash(type obj, int iter)
	{
		return (first_hash(obj) + iter*second_hash(obj)) % _sizes[_actual_size_index];
	}

	type* next(type* it)
	{
		do
		{
			if (it == _table + _sizes[_actual_size_index])
				break;
			++it;
			if (it == _table + _sizes[_actual_size_index])
				break;
			if (get_status(it - _table) == occupied)
				break;
		} while (true);
		return it;
	}

	bool is_rehash_needed()
	{
		return _internal_size > _max_load_factor_size[_actual_size_index];
	}

	void set_status(int i, status stat)
	{
		int main_index = i / 4;
		int rest_index = (i % 4) * 2;


		_status_table[main_index] &= (~(MASK << rest_index));
		
		switch (stat)
		{
		case free:
			_status_table[main_index] |= (SET_FREE << rest_index);
			break;
		case removed:
			_status_table[main_index] |= (SET_REMOVED << rest_index);
			break;
		case occupied:
			_status_table[main_index] |= (SET_OCCUPIED << rest_index);
			break;
		default:
			break;
		}
	}

	status get_status(int i)
	{
		int main_index = i / 4;
		int rest_index = (i % 4)*2;

		unsigned char stat = _status_table[main_index] & (MASK << rest_index);
		stat >>= rest_index;

		status ret = free;

		unsigned char tmp = MASK << 2;
		switch (stat)
		{
		case SET_FREE:
			ret = free;
			break;
		case SET_REMOVED:
			ret = removed;
			break;
		case SET_OCCUPIED:
			ret = occupied;
			break;
		default:
			ret = free;
			break;
		}

		return ret;
	}

	status get_status(int i, unsigned char* tmp_status_table)
	{
		int main_index = (int)floor(i / 4);
		int rest_index = (i % 4) * 2;

		unsigned char stat = tmp_status_table[main_index] & (MASK << rest_index);
		stat >>= rest_index;

		status ret = free;

		switch (stat)
		{
		case SET_FREE:
			ret = free;
			break;
		case SET_REMOVED:
			ret = removed;
			break;
		case SET_OCCUPIED:
			ret = occupied;
			break;
		default:
			break;
		}

		return ret;
	}

	void rehash()
	{
		if (load_factor() < (_max_load_factor / 2.0))
		{
			rehash(_size);
		}
		else
			rehash(_sizes[_actual_size_index + 1]);
	}
};
