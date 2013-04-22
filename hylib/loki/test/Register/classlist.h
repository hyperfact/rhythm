////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Peter K�mmel
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author makes no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////
#ifndef CLASSLIST_H
#define CLASSLIST_H

// $Header: /cvsroot/loki-lib/loki/test/Register/classlist.h,v 1.1 2006/03/08 16:50:18 syntheticpp Exp $

#include <string>

#include <loki/Register.h>


struct Base
{
	virtual void foo() = 0;
};

bool registerClass(std::string, Base*(*)() );

typedef Loki::Seq
<
	struct Foo,
	struct Boo

>::Type ClassList;

#endif
