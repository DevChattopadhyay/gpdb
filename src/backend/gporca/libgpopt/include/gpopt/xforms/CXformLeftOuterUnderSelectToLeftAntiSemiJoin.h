//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformLeftOuterUnderSelectToLeftAntiSemiJoin.h
//
//	@doc:
//		Class to transform logical difference into an aggregate over left anti-semi join
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------
#ifndef GPOPT_CXformSelect2SelectAll_H
#define GPOPT_CXformSelect2SelectAll_H

#include "gpos/base.h"

#include "gpopt/xforms/CXformExploration.h"

namespace gpopt
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CXformDifference2LeftAntiSemiJoin
//
//	@doc:
//		Class to transform logical difference into an aggregate over
//		left anti-semi join
//
//---------------------------------------------------------------------------
class CXformLeftOuterUnderSelectToLeftAntiSemiJoin : public CXformExploration
{
private:
public:
	CXformLeftOuterUnderSelectToLeftAntiSemiJoin(
		const CXformLeftOuterUnderSelectToLeftAntiSemiJoin &) = delete;

	// ctor
	explicit CXformLeftOuterUnderSelectToLeftAntiSemiJoin(CMemoryPool *mp);

	// dtor
	~CXformLeftOuterUnderSelectToLeftAntiSemiJoin() override = default;

	// ident accessors
	EXformId
	Exfid() const override
	{
		return ExfLeftOuterUnderSelectToLeftAntiSemiJoin;
	}

	const CHAR *
	SzId() const override
	{
		return "CXformLeftOuterUnderSelectToLeftAntiSemiJoin";
	}

	// compute xform promise for a given expression handle
	EXformPromise
	Exfp(CExpressionHandle &  // exprhdl
	) const override
	{
		return CXform::ExfpHigh;
	}

	// actual transform
	void Transform(CXformContext *, CXformResult *,
				   CExpression *) const override;

};	// class CXformDifference2LeftAntiSemiJoin

}  // namespace gpopt

#endif	// GPOPT_CXformDSelect2SelectAll_H

// EOF
