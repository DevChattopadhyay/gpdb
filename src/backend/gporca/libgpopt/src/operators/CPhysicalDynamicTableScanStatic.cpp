//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CPhysicalDynamicTableScanStatic.cpp
//
//	@doc:
//		Implementation of dynamic table scan operator
//---------------------------------------------------------------------------

#include "gpopt/operators/CPhysicalDynamicTableScanStatic.h"

#include "gpos/base.h"

#include "gpopt/base/CDistributionSpec.h"
#include "gpopt/base/CDistributionSpecHashed.h"
#include "gpopt/base/CDistributionSpecRandom.h"
#include "gpopt/base/CDistributionSpecSingleton.h"
#include "gpopt/base/CUtils.h"
#include "gpopt/metadata/CName.h"
#include "gpopt/metadata/CTableDescriptor.h"
#include "naucrates/statistics/CStatisticsUtils.h"

using namespace gpopt;


//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicTableScanStatic::CPhysicalDynamicTableScanStatic
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPhysicalDynamicTableScanStatic::CPhysicalDynamicTableScanStatic(
	CMemoryPool *mp, const CName *pnameAlias, CTableDescriptor *ptabdesc,
	ULONG ulOriginOpId, ULONG scan_id, CColRefArray *pdrgpcrOutput,
	CColRef2dArray *pdrgpdrgpcrParts, IMdIdArray *partition_mdids,
	ColRefToUlongMapArray *root_col_mapping_per_part)
	: CPhysicalDynamicScan(mp, ptabdesc, ulOriginOpId, pnameAlias, scan_id,
						   pdrgpcrOutput, pdrgpdrgpcrParts, partition_mdids,
						   root_col_mapping_per_part)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicTableScanStatic::Matches
//
//	@doc:
//		match operator
//
//---------------------------------------------------------------------------
BOOL
CPhysicalDynamicTableScanStatic::Matches(COperator *pop) const
{
	return CUtils::FMatchDynamicScan(this, pop);
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalDynamicTableScanStatic::PstatsDerive
//
//	@doc:
//		Statistics derivation during costing
//
//---------------------------------------------------------------------------
IStatistics *
CPhysicalDynamicTableScanStatic::PstatsDerive(CMemoryPool *mp,
											  CExpressionHandle &exprhdl,
											  CReqdPropPlan *prpplan,
											  IStatisticsArray *	// stats_ctxt
) const
{
	GPOS_ASSERT(nullptr != prpplan);

	return CStatisticsUtils::DeriveStatsForDynamicScan(
		mp, exprhdl, ScanId(), prpplan->Pepp()->PppsRequired());
}


CPartitionPropagationSpec *
CPhysicalDynamicTableScanStatic::PppsDerive(CMemoryPool *mp,
											CExpressionHandle &) const
{
	CPartitionPropagationSpec *pps = GPOS_NEW(mp) CPartitionPropagationSpec(mp);
	pps->Insert(ScanId(), CPartitionPropagationSpec::EpptConsumer,
				Ptabdesc()->MDId(), nullptr, nullptr);

	return pps;
}

// EOF
