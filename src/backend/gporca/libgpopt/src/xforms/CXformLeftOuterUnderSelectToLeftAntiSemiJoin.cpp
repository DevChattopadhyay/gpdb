//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformDifference2LeftAntiSemiJoin.cpp
//
//	@doc:
//		Implementation of the transformation that takes a logical difference and
//		converts it into an aggregate over a left anti-semi join
//---------------------------------------------------------------------------

#include "gpopt/xforms/CXformLeftOuterUnderSelectToLeftAntiSemiJoin.h"

#include "gpos/base.h"

#include "gpopt/exception.h"
#include "gpopt/operators/CLogicalDifference.h"
#include "gpopt/operators/CLogicalLeftOuterJoin.h"
#include "gpopt/operators/CLogicalLeftAntiSemiJoin.h"
#include "gpopt/operators/CPatternLeaf.h"
#include "gpopt/operators/CPatternTree.h"
#include "gpopt/operators/CScalarProjectList.h"
#include "gpopt/xforms/CXformUtils.h"
#include "gpopt/operators/CLogicalSelect.h"
#include "gpopt/base/CColRefSetIter.h"

using namespace gpopt;

//---------------------------------------------------------------------------
//	@function:
//		CXformDifference2LeftAntiSemiJoin::CXformDifference2LeftAntiSemiJoin
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformLeftOuterUnderSelectToLeftAntiSemiJoin::
	CXformLeftOuterUnderSelectToLeftAntiSemiJoin(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CLogicalSelect(mp),
			  GPOS_NEW(mp) CExpression(
				  mp, GPOS_NEW(mp) CLogicalLeftOuterJoin(mp),
				  GPOS_NEW(mp) CExpression(
					  mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // join outer child
				  GPOS_NEW(mp) CExpression(
					  mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // join inner child
				  GPOS_NEW(mp) CExpression(
					  mp, GPOS_NEW(mp) CPatternTree(mp))   // join predicate
				  ),
			  GPOS_NEW(mp) CExpression(
				  mp, GPOS_NEW(mp) CPatternTree(mp))  // predicate list
			  ))
// pattern
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformDifference2LeftAntiSemiJoin::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformLeftOuterUnderSelectToLeftAntiSemiJoin::Transform(CXformContext *pxfctxt,
											 CXformResult *pxfres,
											 CExpression *pexpr) const
{
	GPOS_ASSERT(nullptr != pxfctxt);
	GPOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	GPOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	CExpression *pexprJoin = (*pexpr)[0];
	CExpression *pexprPredicate = (*pexpr)[1];

	CExpression *pexprJoinOuterChild = (*pexprJoin)[0];
	CExpression *pexprJoinInnerChild = (*pexprJoin)[1];
	CExpression *pexprJoinCondition = (*pexprJoin)[2];
	CColRefSet *joinInnerChildOutputColumns = pexprJoinInnerChild->DeriveOutputColumns();

	if (!((CPredicateUtils::FAnd(pexprJoinCondition) ||
		   CUtils::FScalarCmp(pexprJoinCondition)) &&
		  (CPredicateUtils::FAnd(pexprPredicate) ||
		   CUtils::FScalarNullTest(pexprPredicate))))
	{
		return;
	}

	CColRefSet *usedColumnsJoinPredicate = nullptr;
	CColRefSet *usedInnerRelColJoinPred = GPOS_NEW(mp) CColRefSet(mp);

	if(CUtils::FScalarCmp(pexprJoinCondition))
	{
		usedColumnsJoinPredicate = pexprJoinCondition->DeriveUsedColumns();
		CColRefSetIter it(*usedColumnsJoinPredicate);
		while (it.Advance())
		{
			CColRef *pcr = it.Pcr();
			if (joinInnerChildOutputColumns->FMember(pcr))
			{
				usedInnerRelColJoinPred->Include(pcr);
			}
		}
	}
	else
	{
		for(ULONG childIndex=0;childIndex<pexprJoinCondition->Arity();childIndex++)
		{
			CExpression* pexprChild=(*pexprJoinCondition)[childIndex];
			if(CUtils::FScalarCmp(pexprChild))
			{
				usedColumnsJoinPredicate = pexprChild->DeriveUsedColumns();
				CColRefSetIter it(*usedColumnsJoinPredicate);
				while (it.Advance())
				{
					CColRef *pcr = it.Pcr();
					if (joinInnerChildOutputColumns->FMember(pcr))
					{
						usedInnerRelColJoinPred->Include(pcr);
					}
				}
			}
		}
	}

	if(0 == usedInnerRelColJoinPred->Size())
	{
		return;
	}

	CColRefSet *usedColumnsNulltest = nullptr;

	if(CUtils::FScalarNullTest(pexprPredicate))
	{
		usedColumnsNulltest=pexprPredicate->DeriveUsedColumns();
		CColRefSetIter it(*usedColumnsNulltest);
		while (it.Advance())
		{
			CColRef *pcr = it.Pcr();
			if (usedInnerRelColJoinPred->FMember(pcr))
			{
				pexprJoinOuterChild->AddRef();
				pexprJoinInnerChild->AddRef();
				pexprJoinCondition->AddRef();

				CExpression *pexprLASJ =
					GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CLogicalLeftAntiSemiJoin(mp),
											 pexprJoinOuterChild, pexprJoinInnerChild, pexprJoinCondition);

				pxfres->Add(pexprLASJ);
				usedInnerRelColJoinPred->Release();
				return;
			}
		}
	}
	else
	{
		for(ULONG childIndex=0;childIndex<pexprPredicate->Arity();childIndex++)
		{
			CExpression* pexprChild=(*pexprPredicate)[childIndex];
			if(CUtils::FScalarNullTest(pexprChild))
			{
				usedColumnsNulltest = pexprChild->DeriveUsedColumns();
				CColRefSetIter it(*usedColumnsNulltest);
				while (it.Advance())
				{
					CColRef *pcr = it.Pcr();
					if (joinInnerChildOutputColumns->FMember(pcr))
					{
						pexprJoinOuterChild->AddRef();
						pexprJoinInnerChild->AddRef();
						pexprJoinCondition->AddRef();

						CExpression *pexprLASJ =
							GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CLogicalLeftAntiSemiJoin(mp),
													 pexprJoinOuterChild, pexprJoinInnerChild, pexprJoinCondition);
						pxfres->Add(pexprLASJ);
						usedInnerRelColJoinPred->Release();
						return;
					}
				}
			}
		}
	}

	usedInnerRelColJoinPred->Release();
}

// EOF
