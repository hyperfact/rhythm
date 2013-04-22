/********************************************************************
	created:	2012/06/25
	created:	25:6:2012   14:36
	filename: 	assoc_vector.h
	file base:	assoc_vector
	file ext:	h
	author:		huaiyu
	
	purpose:	排序vector实现的关联容器
*********************************************************************/

#ifndef _ASSOC_VECTOR_H_
#define _ASSOC_VECTOR_H_
////////////////////////////////////////////////////////////////////////////////

#pragma once
// Includes go here:
#include "_prolog.h"
#include <vector>
#include <functional>

_HYLIB_BEGIN
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
template<class _K, class _Ty, class _Pr = less<_K>,
	class _A = std::allocator<_Ty> >
	class assoc_vector {
public:
	typedef assoc_vector<_K, _Ty, _Pr, _A> _Myt;
	typedef std::pair<const _K, _Ty> value_type;
	struct _Kfn : public std::unary_function<value_type, _K> {
		const _K& operator()(const value_type& _X) const
		{return (_X.first); }
		};
	class value_compare
		: public std::binary_function<value_type, value_type, bool> {
		friend class assoc_vector<_K, _Ty, _Pr, _A>;
	public:
		bool operator()(const value_type& _X,
			const value_type& _Y) const
			{return (comp(_X.first, _Y.first)); }
		bool operator()(const value_type::first_type& _X,
			const value_type::first_type& _Y) const
			{return (comp(_X, _Y)); }
		bool operator()(const value_type::first_type& _X,
			const value_type& _Y) const
			{return (comp(_X, _Y.first)); }
		bool operator()(const value_type& _X,
			const value_type::first_type& _Y) const
			{return (comp(_X.first, _Y)); }
	protected:
		value_compare(_Pr _Pred)
			: comp(_Pred) {}
		_Pr comp;
		};
	typedef _K key_type;
	typedef _Ty referent_type;
	typedef _Pr key_compare;
	typedef _A allocator_type;
	typedef _A::reference _Tref;
	typedef std::vector<value_type, _A> _Imp;
	typedef _Imp::size_type size_type;
	typedef _Imp::difference_type difference_type;
	typedef _Imp::reference reference;
	typedef _Imp::const_reference const_reference;
	typedef _Imp::iterator iterator;
	typedef _Imp::const_iterator const_iterator;
	typedef _Imp::reverse_iterator reverse_iterator;
	typedef _Imp::const_reverse_iterator const_reverse_iterator;
	typedef std::pair<iterator, bool> _Pairib;
	typedef std::pair<iterator, iterator> _Pairii;
	typedef std::pair<const_iterator, const_iterator> _Paircc;

	explicit assoc_vector(const _Pr& _Pred = _Pr(), const _A& _Al = _A())
		: _Value_compare(_Pred), _Con(_Al) {}
	typedef const value_type *_It;
	assoc_vector(_It _F, _It _L, const _Pr& _Pred = _Pr(),
		const _A& _Al = _A())
		: _Value_compare(_Pred), _Con(_Al)
		{_Con.insert(_Con.end(), _F, _L);
		std::sort(_Con.begin(), _Con.end(), _Value_compare); }
	_Myt& operator=(const _Myt& _X)
		{_Con = _X._Con;
		_Value_compare = _X._Value_compare;
		return (*this); }
	iterator begin()
		{return (_Con.begin()); }
	const_iterator begin() const
		{return (_Con.begin()); }
	iterator end()
		{return (_Con.end()); }
	const_iterator end() const
		{return (_Con.end()); }
	reverse_iterator rbegin()
		{return (_Con.rbegin()); }
	const_reverse_iterator rbegin() const
		{return (_Con.rbegin()); }
	reverse_iterator rend()
		{return (_Con.rend()); }
	const_reverse_iterator rend() const
		{return (_Con.rend()); }
	size_type size() const
		{return (_Con.size()); }
	size_type max_size() const
		{return (_Con.max_size()); }
	bool empty() const
		{return (_Con.empty()); }
	_A get_allocator() const
		{return (_Con.get_allocator()); }
	_Tref operator[](const key_type& _Kv)
		{iterator _P = insert(value_type(_Kv, _Ty())).first;
		return ((*_P).second); }
	_Pairib insert(const value_type& _X)
		{_Pairib _Ans = _Insert(_X);
		return (_Pairib(_Ans.first, _Ans.second)); }
	iterator insert(iterator _P, const value_type& _X)
		{return (_Insert(_P, _X)); }
	void insert(_It _F, _It _L)
		{for (; _F != _L; ++_F)
			_Insert(*_F); }
	iterator erase(iterator _P)
		{return (_Con.erase((_Imp::iterator&)_P)); }
	iterator erase(iterator _F, iterator _L)
		{return (_Con.erase((_Imp::iterator&)_F,
			(_Imp::iterator&)_L)); }
	size_type erase(const _K& _Kv)
		{iterator i(find(_Kv));
		if (i == end())
			{return 0; }
		else
			{erase(i);
			return 1; }}
	void clear()
		{_Con.clear(); }
	void swap(_Myt& _X)
		{std::swap(_Con, _X._Con);
		std::swap(_Value_compare, _X._Value_compare); }
	friend void swap(_Myt& _X, _Myt& _Y)
		{_X.swap(_Y); }
	key_compare key_comp() const
		{return (key_compare()); }
	value_compare value_comp() const
		{return (_Value_compare); }
	iterator find(const _K& _Kv)
		{return (_Tr.find(_Kv)); }
	const_iterator find(const _K& _Kv) const
		{return (_Tr.find(_Kv)); }
	size_type count(const _K& _Kv) const
		{_Paircc _Ans = equal_range(_Kv);
		size_type _N = 0;
		_Distance(_Ans.first, _Ans.second, _N);
		return (_N); }
	iterator lower_bound(const _K& _Kv)
		{return (std::lower_bound(_Con.begin(), _Con.end(), _Kv, _Value_compare)); }
	const_iterator lower_bound(const _K& _Kv) const
		{return (std::lower_bound(_Con.begin(), _Con.end(), _Kv, _Value_compare)); }
	iterator upper_bound(const _K& _Kv)
		{return (std::upper_bound(_Con.begin(), _Con.end(), _Kv, _Value_compare)); }
	const_iterator upper_bound(const _K& _Kv) const
		{return (std::upper_bound(_Con.begin(), _Con.end(), _Kv, _Value_compare)); }
	_Pairii equal_range(const _K& _Kv)
		{return (std::equal_range(_Con.begin(), _Con.end(), _Kv, _Value_compare)); }
	_Paircc equal_range(const _K& _Kv) const
		{return (std::equal_range(_Con.begin(), _Con.end(), _Kv, _Value_compare)); }
protected:
	_Pairib _Insert(const value_type& _X) {
		iterator _I = upper_bound(_X.first);
		if (_Con.end() == _I)
			{_Con.push_back(_X);
			return _Pairib(_Con.end()-1, true); }
		else {
			if (_Value_compare(_X, *_I))
				{_Con.insert(_I, _X);
				return _Pairib(_I, true); }
			else
				{return _Pairib(_I, false); }}}

	iterator _Insert(iterator _P, const value_type& _X) {
		if (size() == 0)
			{return (_Con.insert(_Con.end(), _X)); }
		else if (_P == begin())
			{if (_Value_compare(_X, _P))
				{return (_Con.insert(_P, _X)); }}
		else if (_P == end())
			{if (_Value_compare(_Con.back(), _X))
				{return (_Con.insert(_P, _X)); }}
		else
			{if (_Value_compare(_P, _X)
				&& (_P==end()-1 || (_Value_compare(_X, _P+1)&&!_Value_compare(_P+1, _X))))
				{return (_Con.insert(_P, _X)); }}
		return (_Insert(_X).first); }

protected:
	_Imp			_Con;
	value_compare	_Value_compare;
	};
		// map TEMPLATE OPERATORS
template<class _K, class _Ty, class _Pr, class _A> inline
	bool operator==(const assoc_vector<_K, _Ty, _Pr, _A>& _X,
		const assoc_vector<_K, _Ty, _Pr, _A>& _Y)
	{return (_X.size() == _Y.size()
		&& equal(_X.begin(), _X.end(), _Y.begin())); }
template<class _K, class _Ty, class _Pr, class _A> inline
	bool operator!=(const assoc_vector<_K, _Ty, _Pr, _A>& _X,
		const assoc_vector<_K, _Ty, _Pr, _A>& _Y)
	{return (!(_X == _Y)); }
template<class _K, class _Ty, class _Pr, class _A> inline
	bool operator<(const assoc_vector<_K, _Ty, _Pr, _A>& _X,
		const assoc_vector<_K, _Ty, _Pr, _A>& _Y)
	{return (lexicographical_compare(_X.begin(), _X.end(),
		_Y.begin(), _Y.end())); }
template<class _K, class _Ty, class _Pr, class _A> inline
	bool operator>(const assoc_vector<_K, _Ty, _Pr, _A>& _X,
		const assoc_vector<_K, _Ty, _Pr, _A>& _Y)
	{return (_Y < _X); }
template<class _K, class _Ty, class _Pr, class _A> inline
	bool operator<=(const assoc_vector<_K, _Ty, _Pr, _A>& _X,
		const assoc_vector<_K, _Ty, _Pr, _A>& _Y)
	{return (!(_Y < _X)); }
template<class _K, class _Ty, class _Pr, class _A> inline
	bool operator>=(const assoc_vector<_K, _Ty, _Pr, _A>& _X,
		const assoc_vector<_K, _Ty, _Pr, _A>& _Y)
	{return (!(_X < _Y)); }
// 		// TEMPLATE CLASS multimap
// template<class _K, class _Ty, class _Pr = less<_K>,
// 	class _A = allocator<_Ty> >
// 	class multimap {
// public:
// 	typedef multimap<_K, _Ty, _Pr, _A> _Myt;
// 	typedef pair<const _K, _Ty> value_type;
// 	struct _Kfn : public unary_function<value_type, _K> {
// 		const _K& operator()(const value_type& _X) const
// 		{return (_X.first); }
// 		};
// 	class value_compare
// 		: public binary_function<value_type, value_type, bool> {
// 		friend class map<_K, _Ty, _Pr, _A>;
// 	public:
// 		bool operator()(const value_type& _X,
// 			const value_type& _Y) const
// 			{return (comp(_X.first, _Y.first)); }
// 	_PROTECTED:
// 		value_compare(_Pr _Pred)
// 			: comp(_Pred) {}
// 		_Pr comp;
// 		};
// 	typedef _K key_type;
// 	typedef _Ty referent_type;
// 	typedef _Pr key_compare;
// 	typedef _A allocator_type;
// 	typedef _Tree<_K, value_type, _Kfn, _Pr, _A> _Imp;
// 	typedef _Imp::size_type size_type;
// 	typedef _Imp::difference_type difference_type;
// 	typedef _Imp::reference reference;
// 	typedef _Imp::const_reference const_reference;
// 	typedef _Imp::iterator iterator;
// 	typedef _Imp::const_iterator const_iterator;
// 	typedef _Imp::reverse_iterator reverse_iterator;
// 	typedef _Imp::const_reverse_iterator const_reverse_iterator;
// 	typedef pair<iterator, iterator> _Pairii;
// 	typedef pair<const_iterator, const_iterator> _Paircc;
// 	explicit multimap(const _Pr& _Pred = _Pr(),
// 		const _A& _Al = _A())
// 		: _Tr(_Pred, true, _Al) {}
// 	typedef const value_type *_It;
// 	multimap(_It _F, _It _L, const _Pr& _Pred = _Pr(),
// 			const _A& _Al = _A())
// 		: _Tr(_Pred, true, _Al)
// 		{for (; _F != _L; ++_F)
// 			_Tr.insert(*_F); }
// 	_Myt& operator=(const _Myt& _X)
// 		{_Tr = _X._Tr;
// 		return (*this); }
// 	iterator begin()
// 		{return (_Tr.begin()); }
// 	const_iterator begin() const
// 		{return (_Tr.begin()); }
// 	iterator end()
// 		{return (_Tr.end()); }
// 	const_iterator end() const
// 		{return (_Tr.end()); }
// 	reverse_iterator rbegin()
// 		{return (_Tr.rbegin()); }
// 	const_reverse_iterator rbegin() const
// 		{return (_Tr.rbegin()); }
// 	reverse_iterator rend()
// 		{return (_Tr.rend()); }
// 	const_reverse_iterator rend() const
// 		{return (_Tr.rend()); }
// 	size_type size() const
// 		{return (_Tr.size()); }
// 	size_type max_size() const
// 		{return (_Tr.max_size()); }
// 	bool empty() const
// 		{return (_Tr.empty()); }
// 	_A get_allocator() const
// 		{return (_Tr.get_allocator()); }
// 	iterator insert(const value_type& _X)
// 		{return (_Tr.insert(_X).first); }
// 	iterator insert(iterator _P, const value_type& _X)
// 		{return (_Tr.insert((_Imp::iterator&)_P, _X)); }
// 	void insert(_It _F, _It _L)
// 		{for (; _F != _L; ++_F)
// 			_Tr.insert(*_F); }
// 	iterator erase(iterator _P)
// 		{return (_Tr.erase((_Imp::iterator&)_P)); }
// 	iterator erase(iterator _F, iterator _L)
// 		{return (_Tr.erase((_Imp::iterator&)_F,
// 			(_Imp::iterator&)_L)); }
// 	size_type erase(const _K& _Kv = _K())
// 		{return (_Tr.erase(_Kv)); }
// 	void clear()
// 		{_Tr.clear(); }
// 	void swap(_Myt& _X)
// 		{std::swap(_Tr, _X._Tr); }
// 	friend void swap(_Myt& _X, _Myt& _Y)
// 		{_X.swap(_Y); }
// 	key_compare key_comp() const
// 		{return (_Tr.key_comp()); }
// 	value_compare value_comp() const
// 		{return (value_compare(_Tr.key_comp())); }
// 	iterator find(const _K& _Kv)
// 		{return (_Tr.find(_Kv)); }
// 	const_iterator find(const _K& _Kv) const
// 		{return (_Tr.find(_Kv)); }
// 	size_type count(const _K& _Kv) const
// 		{return (_Tr.count(_Kv)); }
// 	iterator lower_bound(const _K& _Kv)
// 		{return (_Tr.lower_bound(_Kv)); }
// 	const_iterator lower_bound(const _K& _Kv) const
// 		{return (_Tr.lower_bound(_Kv)); }
// 	iterator upper_bound(const _K& _Kv)
// 		{return (_Tr.upper_bound(_Kv)); }
// 	const_iterator upper_bound(const _K& _Kv) const
// 		{return (_Tr.upper_bound(_Kv)); }
// 	_Pairii equal_range(const _K& _Kv)
// 		{return (_Tr.equal_range(_Kv)); }
// 	_Paircc equal_range(const _K& _Kv) const
// 		{return (_Tr.equal_range(_Kv)); }
// protected:
// 	_Imp _Tr;
// 	};
// 		// multimap TEMPLATE OPERATORS
// template<class _K, class _Ty, class _Pr, class _A> inline
// 	bool operator==(const multimap<_K, _Ty, _Pr, _A>& _X,
// 		const multimap<_K, _Ty, _Pr, _A>& _Y)
// 	{return (_X.size() == _Y.size()
// 		&& equal(_X.begin(), _X.end(), _Y.begin())); }
// template<class _K, class _Ty, class _Pr, class _A> inline
// 	bool operator!=(const multimap<_K, _Ty, _Pr, _A>& _X,
// 		const multimap<_K, _Ty, _Pr, _A>& _Y)
// 	{return (!(_X == _Y)); }
// template<class _K, class _Ty, class _Pr, class _A> inline
// 	bool operator<(const multimap<_K, _Ty, _Pr, _A>& _X,
// 		const multimap<_K, _Ty, _Pr, _A>& _Y)
// 	{return (lexicographical_compare(_X.begin(), _X.end(),
// 		_Y.begin(), _Y.end())); }
// template<class _K, class _Ty, class _Pr, class _A> inline
// 	bool operator>(const multimap<_K, _Ty, _Pr, _A>& _X,
// 		const multimap<_K, _Ty, _Pr, _A>& _Y)
// 	{return (_Y < _X); }
// template<class _K, class _Ty, class _Pr, class _A> inline
// 	bool operator<=(const multimap<_K, _Ty, _Pr, _A>& _X,
// 		const multimap<_K, _Ty, _Pr, _A>& _Y)
// 	{return (!(_Y < _X)); }
// template<class _K, class _Ty, class _Pr, class _A> inline
// 	bool operator>=(const multimap<_K, _Ty, _Pr, _A>& _X,
// 		const multimap<_K, _Ty, _Pr, _A>& _Y)
// 	{return (!(_X < _Y)); }

////////////////////////////////////////////////////////////////////////////////
_HYLIB_END

#endif // _ASSOC_VECTOR_H_
