//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalDynamicTableScan.h
//
//	@doc:
//		Dynamic Table scan operator
//---------------------------------------------------------------------------
#ifndef GPOPT_CPhysicalDynamicTableScanStatic_H
#define GPOPT_CPhysicalDynamicTableScanStatic_H

#include "gpos/base.h"

#include "gpopt/operators/CPhysicalDynamicScan.h"

namespace gpopt
{
//---------------------------------------------------------------------------
//	@class:
//		CPhysicalDynamicTableScanStatic
//
//	@doc:
//		Dynamic Table scan operator
//
//---------------------------------------------------------------------------
class CPhysicalDynamicTableScanStatic : public CPhysicalDynamicScan
{
private:
public:
	CPhysicalDynamicTableScanStatic(const CPhysicalDynamicTableScanStatic &) = delete;

	// ctors
	CPhysicalDynamicTableScanStatic(CMemoryPool *mp, const CName *pnameAlias,
									CTableDescriptor *ptabdesc, ULONG ulOriginOpId,
									ULONG scan_id, CColRefArray *pdrgpcrOutput,
									CColRef2dArray *pdrgpdrgpcrParts,
									IMdIdArray *partition_mdids,
									ColRefToUlongMapArray *root_col_mapping_per_part);

	// ident accessors
	EOperatorId
	Eopid() const override
	{
		return EopPhysicalDynamicTableScanStatic;
	}

	// return a string for operator name
	const CHAR *
	SzId() const override
	{
		return "CPhysicalDynamicTableScanStatic";
	}

	// match function
	BOOL Matches(COperator *) const override;

	// statistics derivation during costing
	IStatistics *PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
							  CReqdPropPlan *prpplan,
							  IStatisticsArray *stats_ctxt) const override;

	// conversion function
	static CPhysicalDynamicTableScanStatic *
	PopConvert(COperator *pop)
	{
		GPOS_ASSERT(nullptr != pop);
		GPOS_ASSERT(EopPhysicalDynamicTableScanStatic == pop->Eopid());

		return dynamic_cast<CPhysicalDynamicTableScanStatic *>(pop);
	}

	CPartitionPropagationSpec *PppsDerive(
		CMemoryPool *mp, CExpressionHandle &exprhdl) const override;

};	// class CPhysicalDynamicTableScanStatic

}  // namespace gpopt

#endif	// !GPOPT_CPhysicalDynamicTableScanStatic_H

// EOF
