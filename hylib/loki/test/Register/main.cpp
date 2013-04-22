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

// $Header: /cvsroot/loki-lib/loki/test/Register/main.cpp,v 1.2 2006/03/08 17:07:18 syntheticpp Exp $

#include <iostream>

#include <loki/Factory.h>
#include <loki/Singleton.h>

#include "classlist.h"

typedef Loki::SingletonHolder
<
	Loki::Factory<Base, std::string>
>
BaseFactory;


bool registerClass(std::string key, Base*(*creator)() )
{
	return BaseFactory::Instance().Register(key,creator);
}

Loki::RegisterOnCreateSet<ClassList> registerAllClasses;
Loki::UnRegisterOnDeleteSet<ClassList> unregisterAllClasses;


int main()
{
	Base* foo = BaseFactory::Instance().CreateObject("Foo");
	Base* boo = BaseFactory::Instance().CreateObject("Boo");
    
	foo->foo();
	boo->foo();

	delete foo;
	delete boo;

#if defined(__BORLANDC__) || defined(_MSC_VER)
    system("PAUSE");
#endif

	return 0;
}
