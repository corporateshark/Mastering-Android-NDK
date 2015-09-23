#pragma once

#include "IntrusivePtr.h"
#include "UI/UIView.h"

class clVerticalLayout: public clUIView
{
public:
	virtual void LayoutChildViews() override;
};

class clHorizontalLayout: public clUIView
{
public:
	virtual void LayoutChildViews() override;
};
