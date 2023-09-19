//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformDynamicGet2DynamicTableScan.h
//
//	@doc:
//		Transform DynamicGet to DynamicTableScan
//---------------------------------------------------------------------------
#ifndef GPOPT_CXformDynamicGet2DynamicTableScanStatic_H
#define GPOPT_CXformDynamicGet2DynamicTableScanStatic_H

#include "gpos/base.h"

#include "gpopt/operators/CLogicalDynamicGet.h"
#include "gpopt/xforms/CXformImplementation.h"

namespace gpopt
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CXformDynamicGet2DynamicTableScanStatic
//
//	@doc:
//		Transform DynamicGet to DynamicTableScan
//
//---------------------------------------------------------------------------
class CXformDynamicGet2DynamicTableScanStatic : public CXformImplementation
{
private:
public:
	CXformDynamicGet2DynamicTableScanStatic(
		const CXformDynamicGet2DynamicTableScanStatic &) = delete;

	// ctor
	explicit CXformDynamicGet2DynamicTableScanStatic(CMemoryPool *mp);

	// dtor
	~CXformDynamicGet2DynamicTableScanStatic() override = default;

	// ident accessors
	EXformId
	Exfid() const override
	{
		return ExfDynamicGet2DynamicTableScanStatic;
	}

	// return a string for xform name
	const CHAR *
	SzId() const override
	{
		return "CXformDynamicGet2DynamicTableScanStatic";
	}

	// compute xform promise for a given expression handle
	EXformPromise Exfp(CExpressionHandle &exprhdl) const override;

	// actual transform
	void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
				   CExpression *pexpr) const override;

};	// class CXformDynamicGet2DynamicTableScanStatic

}  // namespace gpopt


#endif	// !GPOPT_CXformDynamicGet2DynamicTableScanStatic_H

// EOF
