/////////////////////////////////////////////////
//  Created by Chris Hinsley.
//  Copyright (C) 2008-2014. All rights reserved.
/////////////////////////////////////////////////

#ifndef __list_1234__
#define __list_1234__

class Listnode
{
private:
	Listnode *_succ;
	Listnode *_pred;
	void *_obj;
public:
	explicit Listnode(void *o);
	
	void *get_object() const;
	void set_succ(Listnode *node);
	void set_pred(Listnode *node);
	Listnode *get_succ() const;
	Listnode *get_pred() const;
	void add_node_after(Listnode *node);
	void add_node_before(Listnode *node);
	void remove_node() const;
	bool is_first() const;
	bool is_last() const;
};

class Listhead
{
private:
	Listnode *_head;
	Listnode *_tail;
	Listnode *_tailpred;
public:
	explicit Listhead();
	
	void reset();
	bool is_empty();
	void add_at_head(Listnode *node);
	void add_at_tail(Listnode *node);
	Listnode *remove_head();
	Listnode *remove_tail();
	Listnode *get_head() const;
	Listnode *get_tail() const;
	Listnode *get_node_at_index(unsigned int i);
	unsigned int get_index_of_node(Listnode *node);
	Listnode *enumerate_forwards(Listnode * (*callback)(Listhead *, Listnode *, void *), void *user);
	Listnode *enumerate_backwards(Listnode * (*callback)(Listhead *, Listnode *, void *), void *user);
};

inline Listnode::Listnode(void *o)
: _obj(o)
{}

inline void *Listnode::get_object() const
{
	return _obj;
}

inline void Listnode::set_succ(Listnode *node)
{
	_succ = node;
}

inline void Listnode::set_pred(Listnode *node)
{
	_pred = node;
}

inline Listnode *Listnode::get_succ() const
{
	return _succ;
}

inline Listnode *Listnode::get_pred() const
{
	return _pred;
}

inline void Listnode::add_node_after(Listnode *node)
{
	_succ->_pred = node;
	node->_succ = _succ;
	node->_pred = this;
	_succ = node;
}

inline void Listnode::add_node_before(Listnode *node)
{
	_pred->_succ = node;
	node->_succ = this;
	node->_pred = _pred;
	_pred = node;
}

inline void Listnode::remove_node() const
{
	_succ->_pred = _pred;
	_pred->_succ = _succ;
}

inline bool Listnode::is_first() const
{
	return (_pred->_pred == 0);
}

inline bool Listnode::is_last() const
{
	return (_succ->_succ == 0);
}

inline Listhead::Listhead()
{
	_head = (Listnode *)&_tail;
	_tail = 0;
	_tailpred = (Listnode *)&_head;
}

inline void Listhead::reset()
{
	_head = (Listnode *)&_tail;
	_tail = 0;
	_tailpred = (Listnode *)&_head;
}

inline void Listhead::add_at_head(Listnode *node)
{
	node->set_succ(_head);
	node->set_pred((Listnode *)&_head);
	_head->set_pred(node);
	_head = node;
}

inline void Listhead::add_at_tail(Listnode *node)
{
	node->set_succ((Listnode *)&_tail);
	node->set_pred(_tailpred);
	_tailpred->set_succ(node);
	_tailpred = node;
}

inline Listnode *Listhead::remove_head()
{
	if (_head == (Listnode *)&_tail)
	{
		return 0;
	}
	Listnode *node = _head;
	node->remove_node();
	return node;
}

inline Listnode *Listhead::remove_tail()
{
	if (_tailpred == (Listnode *)&_head)
	{
		return 0;
	}
	Listnode *node = _tailpred;
	node->remove_node();
	return node;
}

inline bool Listhead::is_empty()
{
	return (_head == (Listnode *)&_tail);
}

inline Listnode *Listhead::get_head() const
{
	return _head;
}

inline Listnode *Listhead::get_tail() const
{
	return _tailpred;
}

#endif
