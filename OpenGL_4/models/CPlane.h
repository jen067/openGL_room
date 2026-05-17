#pragma once
#include "CShape.h"

class CPlane : public CShape
{
private:

public:
	CPlane();
	virtual ~CPlane();
	virtual void drawRaw() override;
	virtual void reset() override;

};