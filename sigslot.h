/////////////////////////////////////////////////
//  Created by Chris Hinsley.
//  Copyright (C) 2008-2014. All rights reserved.
/////////////////////////////////////////////////

#ifndef _sigslot_1234_
#define _sigslot_1234_

#include "list.h"

class has_slots_base
{
private:
	Listhead _slot_list;

public:
	has_slots_base()
	{}

	virtual ~has_slots_base()
	{}

	Listhead *get_slot_list()
	{
		return &_slot_list;
	}
};

class signal_base
{
private:
	Listhead _signal_list;

public:
	signal_base()
	{}

	virtual ~signal_base()
	{}

	Listhead *get_signal_list()
	{
		return &_signal_list;
	}
};

class connection_base
{
private:
	Listnode _signal_node;
	Listnode _slot_node;

public:
	connection_base(signal_base *sig, has_slots_base *slot)
		: _signal_node(this)
		, _slot_node(this)
	{
		sig->get_signal_list()->add_at_tail(&_signal_node);
		slot->get_slot_list()->add_at_tail(&_slot_node);
	}

	virtual ~connection_base()
	{
		_signal_node.remove_node();
		_slot_node.remove_node();
	}
};

class has_slots
	: public has_slots_base
{
public:
	has_slots()
	{}

	virtual ~has_slots()
	{
		disconnect();
	}

	void disconnect()
	{
		Listnode *next, *node;
		next = get_slot_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			delete (connection_base *)node->get_object();
		}
	}
};

class signal
	: public signal_base
{
public:
	signal()
	{}

	virtual ~signal()
	{
		disconnect();
	}

	void disconnect()
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			delete (connection_base *)node->get_object();
		}
	}
};

//8 paramater signal stuff

template<class arg1_type, class arg2_type, class arg3_type, class arg4_type, class arg5_type, class arg6_type, class arg7_type, class arg8_type>
class connection_base8
	: public connection_base
{
public:
	connection_base8(signal_base *sig, has_slots_base *slot)
		: connection_base(sig, slot)
	{}

	virtual ~connection_base8()
	{}

	virtual void emit(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type) = 0;
};

template<class slot_type, class arg1_type, class arg2_type, class arg3_type, class arg4_type, class arg5_type, class arg6_type, class arg7_type, class arg8_type>
class connection8
	: public connection_base8<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type>
{
	typedef void (slot_type::*memfun)(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type);

private:
	slot_type *_slot;
	memfun _func;

public:
	connection8(signal *sig, slot_type *slot, memfun func)
		: connection_base8<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type>(sig, slot)
		, _slot(slot)
		, _func(func)
	{}

	virtual ~connection8()
	{}

	slot_type *get_slot()
	{
		return _slot;
	}

	memfun get_func()
	{
		return _func;
	}

	virtual void emit(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7, arg8_type a8)
	{
		(_slot->*_func)(a1, a2, a3, a4, a5, a6, a7, a8);
	}
};

template<class arg1_type, class arg2_type, class arg3_type, class arg4_type, class arg5_type, class arg6_type, class arg7_type, class arg8_type>
class signal8
	: public signal
{
public:
	signal8()
	{}

	virtual ~signal8()
	{}

	template<class slot_type>
	void connect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type))
	{
		new connection8<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type>(this, slot, func);
	}

	template<class slot_type>
	void disconnect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type))
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection8<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type> *con
				= (connection8<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type> *)node->get_object();
			if ((con->get_slot() == slot) && (con->get_func() == func))
			{
				delete con;
			}
		}
	}

	template<class slot_type>
	void disconnect(slot_type *slot)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection8<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type> *con
				= (connection8<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type> *)node->get_object();
			if (con->get_slot() == slot)
			{
				delete con;
			}
		}
	}

	void emit(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7, arg8_type a8)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection_base8<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type> *con
				= (connection_base8<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type> *)node->get_object();
			con->emit(a1, a2, a3, a4, a5, a6, a7, a8);
		}
	}
};

//7 paramater signal stuff

template<class arg1_type, class arg2_type, class arg3_type, class arg4_type, class arg5_type, class arg6_type, class arg7_type>
class connection_base7
	: public connection_base
{
public:
	connection_base7(signal_base *sig, has_slots_base *slot)
		: connection_base(sig, slot)
	{}

	virtual ~connection_base7()
	{}

	virtual void emit(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type) = 0;
};

template<class slot_type, class arg1_type, class arg2_type, class arg3_type, class arg4_type, class arg5_type, class arg6_type, class arg7_type>
class connection7
	: public connection_base7<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type>
{
	typedef void (slot_type::*memfun)(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type);

private:
	slot_type *_slot;
	memfun _func;

public:
	connection7(signal *sig, slot_type *slot, memfun func)
		: connection_base7<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type>(sig, slot)
		, _slot(slot)
		, _func(func)
	{}

	virtual ~connection7()
	{}

	slot_type *get_slot()
	{
		return _slot;
	}

	memfun get_func()
	{
		return _func;
	}

	virtual void emit(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7)
	{
		(_slot->*_func)(a1, a2, a3, a4, a5, a6, a7);
	}
};

template<class arg1_type, class arg2_type, class arg3_type, class arg4_type, class arg5_type, class arg6_type, class arg7_type>
class signal7
	: public signal
{
public:
	signal7()
	{}

	virtual ~signal7()
	{}

	template<class slot_type>
	void connect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type))
	{
		new connection7<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type>(this, slot, func);
	}

	template<class slot_type>
	void disconnect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type))
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection7<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type> *con
				= (connection7<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type> *)node->get_object();
			if ((con->get_slot() == slot) && (con->get_func() == func))
			{
				delete con;
			}
		}
	}

	template<class slot_type>
	void disconnect(slot_type *slot)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection7<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type> *con
				= (connection7<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type> *)node->get_object();
			if (con->get_slot() == slot)
			{
				delete con;
			}
		}
	}

	void emit(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection_base7<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type> *con
				= (connection_base7<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type> *)node->get_object();
			con->emit(a1, a2, a3, a4, a5, a6, a7);
		}
	}
};

//6 paramater signal stuff

template<class arg1_type, class arg2_type, class arg3_type, class arg4_type, class arg5_type, class arg6_type>
class connection_base6
	: public connection_base
{
public:
	connection_base6(signal_base *sig, has_slots_base *slot)
		: connection_base(sig, slot)
	{}

	virtual ~connection_base6()
	{}

	virtual void emit(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type) = 0;
};

template<class slot_type, class arg1_type, class arg2_type, class arg3_type, class arg4_type, class arg5_type, class arg6_type>
class connection6
	: public connection_base6<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type>
{
	typedef void (slot_type::*memfun)(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type);

private:
	slot_type *_slot;
	memfun _func;

public:
	connection6(signal *sig, slot_type *slot, memfun func)
		: connection_base6<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type>(sig, slot)
		, _slot(slot)
		, _func(func)
	{}

	virtual ~connection6()
	{}

	slot_type *get_slot()
	{
		return _slot;
	}

	memfun get_func()
	{
		return _func;
	}

	virtual void emit(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6)
	{
		(_slot->*_func)(a1, a2, a3, a4, a5, a6);
	}
};

template<class arg1_type, class arg2_type, class arg3_type, class arg4_type, class arg5_type, class arg6_type>
class signal6
	: public signal
{
public:
	signal6()
	{}

	virtual ~signal6()
	{}

	template<class slot_type>
	void connect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type))
	{
		new connection6<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type>(this, slot, func);
	}

	template<class slot_type>
	void disconnect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type))
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection6<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type> *con
				= (connection6<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type> *)node->get_object();
			if ((con->get_slot() == slot) && (con->get_func() == func))
			{
				delete con;
			}
		}
	}

	template<class slot_type>
	void disconnect(slot_type *slot)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection6<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type> *con
				= (connection6<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type> *)node->get_object();
			if (con->get_slot() == slot)
			{
				delete con;
			}
		}
	}

	void emit(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection_base6<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type> *con
				= (connection_base6<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type> *)node->get_object();
			con->emit(a1, a2, a3, a4, a5, a6);
		}
	}
};

//5 paramater signal stuff

template<class arg1_type, class arg2_type, class arg3_type, class arg4_type, class arg5_type>
class connection_base5
	: public connection_base
{
public:
	connection_base5(signal_base *sig, has_slots_base *slot)
		: connection_base(sig, slot)
	{}

	virtual ~connection_base5()
	{}

	virtual void emit(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type) = 0;
};

template<class slot_type, class arg1_type, class arg2_type, class arg3_type, class arg4_type, class arg5_type>
class connection5
	: public connection_base5<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type>
{
	typedef void (slot_type::*memfun)(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type);

private:
	slot_type *_slot;
	memfun _func;

public:
	connection5(signal *sig, slot_type *slot, memfun func)
		: connection_base5<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type>(sig, slot)
		, _slot(slot)
		, _func(func)
	{}

	virtual ~connection5()
	{}

	slot_type *get_slot()
	{
		return _slot;
	}

	memfun get_func()
	{
		return _func;
	}

	virtual void emit(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5)
	{
		(_slot->*_func)(a1, a2, a3, a4, a5);
	}
};

template<class arg1_type, class arg2_type, class arg3_type, class arg4_type, class arg5_type>
class signal5
	: public signal
{
public:
	signal5()
	{}

	virtual ~signal5()
	{}

	template<class slot_type>
	void connect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type))
	{
		new connection5<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type>(this, slot, func);
	}

	template<class slot_type>
	void disconnect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type, arg3_type, arg4_type, arg5_type))
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection5<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type> *con
				= (connection5<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type> *)node->get_object();
			if ((con->get_slot() == slot) && (con->get_func() == func))
			{
				delete con;
			}
		}
	}

	template<class slot_type>
	void disconnect(slot_type *slot)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection5<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type> *con
				= (connection5<slot_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type> *)node->get_object();
			if (con->get_slot() == slot)
			{
				delete con;
			}
		}
	}

	void emit(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection_base5<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type> *con
				= (connection_base5<arg1_type, arg2_type, arg3_type, arg4_type, arg5_type> *)node->get_object();
			con->emit(a1, a2, a3, a4, a5);
		}
	}
};

//4 paramater signal stuff

template<class arg1_type, class arg2_type, class arg3_type, class arg4_type>
class connection_base4
	: public connection_base
{
public:
	connection_base4(signal_base *sig, has_slots_base *slot)
		: connection_base(sig, slot)
	{}

	virtual ~connection_base4()
	{}

	virtual void emit(arg1_type, arg2_type, arg3_type, arg4_type) = 0;
};

template<class slot_type, class arg1_type, class arg2_type, class arg3_type, class arg4_type>
class connection4
	: public connection_base4<arg1_type, arg2_type, arg3_type, arg4_type>
{
	typedef void (slot_type::*memfun)(arg1_type, arg2_type, arg3_type, arg4_type);

private:
	slot_type *_slot;
	memfun _func;

public:
	connection4(signal *sig, slot_type *slot, memfun func)
		: connection_base4<arg1_type, arg2_type, arg3_type, arg4_type>(sig, slot)
		, _slot(slot)
		, _func(func)
	{}

	virtual ~connection4()
	{}

	slot_type *get_slot()
	{
		return _slot;
	}

	memfun get_func()
	{
		return _func;
	}

	virtual void emit(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4)
	{
		(_slot->*_func)(a1, a2, a3, a4);
	}
};

template<class arg1_type, class arg2_type, class arg3_type, class arg4_type>
class signal4
	: public signal
{
public:
	signal4()
	{}

	virtual ~signal4()
	{}

	template<class slot_type>
	void connect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type, arg3_type, arg4_type))
	{
		new connection4<slot_type, arg1_type, arg2_type, arg3_type, arg4_type>(this, slot, func);
	}

	template<class slot_type>
	void disconnect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type, arg3_type, arg4_type))
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection4<slot_type, arg1_type, arg2_type, arg3_type, arg4_type> *con
				= (connection4<slot_type, arg1_type, arg2_type, arg3_type, arg4_type> *)node->get_object();
			if ((con->get_slot() == slot) && (con->get_func() == func))
			{
				delete con;
			}
		}
	}

	template<class slot_type>
	void disconnect(slot_type *slot)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection4<slot_type, arg1_type, arg2_type, arg3_type, arg4_type> *con
				= (connection4<slot_type, arg1_type, arg2_type, arg3_type, arg4_type> *)node->get_object();
			if (con->get_slot() == slot)
			{
				delete con;
			}
		}
	}

	void emit(arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection_base4<arg1_type, arg2_type, arg3_type, arg4_type> *con
				= (connection_base4<arg1_type, arg2_type, arg3_type, arg4_type> *)node->get_object();
			con->emit(a1, a2, a3, a4);
		}
	}
};

//3 paramater signal stuff

template<class arg1_type, class arg2_type, class arg3_type>
class connection_base3
	: public connection_base
{
public:
	connection_base3(signal_base *sig, has_slots_base *slot)
		: connection_base(sig, slot)
	{}

	virtual ~connection_base3()
	{}

	virtual void emit(arg1_type, arg2_type, arg3_type) = 0;
};

template<class slot_type, class arg1_type, class arg2_type, class arg3_type>
class connection3
	: public connection_base3<arg1_type, arg2_type, arg3_type>
{
	typedef void (slot_type::*memfun)(arg1_type, arg2_type, arg3_type);

private:
	slot_type *_slot;
	memfun _func;

public:
	connection3(signal *sig, slot_type *slot, memfun func)
		: connection_base3<arg1_type, arg2_type, arg3_type>(sig, slot)
		, _slot(slot)
		, _func(func)
	{}

	virtual ~connection3()
	{}

	slot_type *get_slot()
	{
		return _slot;
	}

	memfun get_func()
	{
		return _func;
	}

	virtual void emit(arg1_type a1, arg2_type a2, arg3_type a3)
	{
		(_slot->*_func)(a1, a2, a3);
	}
};

template<class arg1_type, class arg2_type, class arg3_type>
class signal3
	: public signal
{
public:
	signal3()
	{}

	virtual ~signal3()
	{}

	template<class slot_type>
	void connect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type, arg3_type))
	{
		new connection3<slot_type, arg1_type, arg2_type, arg3_type>(this, slot, func);
	}

	template<class slot_type>
	void disconnect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type, arg3_type))
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection3<slot_type, arg1_type, arg2_type, arg3_type> *con
				= (connection3<slot_type, arg1_type, arg2_type, arg3_type> *)node->get_object();
			if ((con->get_slot() == slot) && (con->get_func() == func))
			{
				delete con;
			}
		}
	}

	template<class slot_type>
	void disconnect(slot_type *slot)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection3<slot_type, arg1_type, arg2_type, arg3_type> *con
				= (connection3<slot_type, arg1_type, arg2_type, arg3_type> *)node->get_object();
			if (con->get_slot() == slot)
			{
				delete con;
			}
		}
	}

	void emit(arg1_type a1, arg2_type a2, arg3_type a3)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection_base3<arg1_type, arg2_type, arg3_type> *con
				= (connection_base3<arg1_type, arg2_type, arg3_type> *)node->get_object();
			con->emit(a1, a2, a3);
		}
	}
};

//2 paramater signal stuff

template<class arg1_type, class arg2_type>
class connection_base2
	: public connection_base
{
public:
	connection_base2(signal_base *sig, has_slots_base *slot)
		: connection_base(sig, slot)
	{}

	virtual ~connection_base2()
	{}

	virtual void emit(arg1_type, arg2_type) = 0;
};

template<class slot_type, class arg1_type, class arg2_type>
class connection2
	: public connection_base2<arg1_type, arg2_type>
{
	typedef void (slot_type::*memfun)(arg1_type, arg2_type);

private:
	slot_type *_slot;
	memfun _func;

public:
	connection2(signal *sig, slot_type *slot, memfun func)
		: connection_base2<arg1_type, arg2_type>(sig, slot)
		, _slot(slot)
		, _func(func)
	{}

	virtual ~connection2()
	{}

	slot_type *get_slot()
	{
		return _slot;
	}

	memfun get_func()
	{
		return _func;
	}

	virtual void emit(arg1_type a1, arg2_type a2)
	{
		(_slot->*_func)(a1, a2);
	}
};

template<class arg1_type, class arg2_type>
class signal2
	: public signal
{
public:
	signal2()
	{}

	virtual ~signal2()
	{}

	template<class slot_type>
	void connect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type))
	{
		new connection2<slot_type, arg1_type, arg2_type>(this, slot, func);
	}

	template<class slot_type>
	void disconnect(slot_type *slot, void (slot_type::*func)(arg1_type, arg2_type))
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection2<slot_type, arg1_type, arg2_type> *con
				= (connection2<slot_type, arg1_type, arg2_type> *)node->get_object();
			if ((con->get_slot() == slot) && (con->get_func() == func))
			{
				delete con;
			}
		}
	}

	template<class slot_type>
	void disconnect(slot_type *slot)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection2<slot_type, arg1_type, arg2_type> *con
				= (connection2<slot_type, arg1_type, arg2_type> *)node->get_object();
			if (con->get_slot() == slot)
			{
				delete con;
			}
		}
	}

	void emit(arg1_type a1, arg2_type a2)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection_base2<arg1_type, arg2_type> *con
				= (connection_base2<arg1_type, arg2_type> *)node->get_object();
			con->emit(a1, a2);
		}
	}
};

//1 paramater signal stuff

template<class arg1_type>
class connection_base1
	: public connection_base
{
public:
	connection_base1(signal_base *sig, has_slots_base *slot)
		: connection_base(sig, slot)
	{}

	virtual ~connection_base1()
	{}

	virtual void emit(arg1_type) = 0;
};

template<class slot_type, class arg1_type>
class connection1
	: public connection_base1<arg1_type>
{
	typedef void (slot_type::*memfun)(arg1_type);

private:
	slot_type *_slot;
	memfun _func;

public:
	connection1(signal *sig, slot_type *slot, memfun func)
		: connection_base1<arg1_type>(sig, slot)
		, _slot(slot)
		, _func(func)
	{}

	virtual ~connection1()
	{}

	slot_type *get_slot()
	{
		return _slot;
	}

	memfun get_func()
	{
		return _func;
	}

	virtual void emit(arg1_type a1)
	{
		(_slot->*_func)(a1);
	}
};

template<class arg1_type>
class signal1
	: public signal
{
public:
	signal1()
	{}

	virtual ~signal1()
	{}

	template<class slot_type>
	void connect(slot_type *slot, void (slot_type::*func)(arg1_type))
	{
		new connection1<slot_type, arg1_type>(this, slot, func);
	}

	template<class slot_type>
	void disconnect(slot_type *slot, void (slot_type::*func)(arg1_type))
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection1<slot_type, arg1_type> *con
				= (connection1<slot_type, arg1_type> *)node->get_object();
			if ((con->get_slot() == slot) && (con->get_func() == func))
			{
				delete con;
			}
		}
	}

	template<class slot_type>
	void disconnect(slot_type *slot)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection1<slot_type, arg1_type> *con
				= (connection1<slot_type, arg1_type> *)node->get_object();
			if (con->get_slot() == slot)
			{
				delete con;
			}
		}
	}

	void emit(arg1_type a1)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection_base1<arg1_type> *con
				= (connection_base1<arg1_type> *)node->get_object();
			con->emit(a1);
		}
	}
};

//0 paramater signal stuff

class connection_base0
	: public connection_base
{
public:
	connection_base0(signal_base *sig, has_slots_base *slot)
		: connection_base(sig, slot)
	{}

	virtual ~connection_base0()
	{}

	virtual void emit() = 0;
};

template<class slot_type>
class connection0
	: public connection_base0
{
	typedef void (slot_type::*memfun)();

private:
	slot_type *_slot;
	memfun _func;

public:
	connection0(signal *sig, slot_type *slot, memfun func)
		: connection_base0(sig, slot)
		, _slot(slot)
		, _func(func)
	{}

	virtual ~connection0()
	{}

	slot_type *get_slot()
	{
		return _slot;
	}

	memfun get_func()
	{
		return _func;
	}

	virtual void emit()
	{
		(_slot->*_func)();
	}
};

class signal0
	: public signal
{
public:
	signal0()
	{}

	virtual ~signal0()
	{}

	template<class slot_type>
	void connect(slot_type *slot, void (slot_type::*func)())
	{
		new connection0<slot_type>(this, slot, func);
	}

	template<class slot_type>
	void disconnect(slot_type *slot, void (slot_type::*func)())
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection0<slot_type> *con
				= (connection0<slot_type> *)node->get_object();
			if ((con->get_slot() == slot) && (con->get_func() == func))
			{
				delete con;
			}
		}
	}

	template<class slot_type>
	void disconnect(slot_type *slot)
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection0<slot_type> *con
				= (connection0<slot_type> *)node->get_object();
			if (con->get_slot() == slot)
			{
				delete con;
			}
		}
	}

	void emit()
	{
		Listnode *next, *node;
		next = get_signal_list()->get_head();
		for (;; )
		{
			node = next;
			next = next->get_succ();
			if (!next)
			{
				break;
			}

			connection_base0 *con
				= (connection_base0 *)node->get_object();
			con->emit();
		}
	}
};

#endif //_sigslot_1234_
