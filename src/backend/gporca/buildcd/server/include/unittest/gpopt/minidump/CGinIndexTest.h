//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal Software, Inc.
//---------------------------------------------------------------------------
#ifndef GPOPT_CGinIndexTest_H
#define GPOPT_CGinIndexTest_H

#include "gpos/base.h"

namespace gpopt
{
	class CGinIndexTest
	{
		private:

			// counter used to mark last successful test
			static
			gpos::ULONG m_ulTestCounter;

		public:

			// unittests
			static
			gpos::GPOS_RESULT EresUnittest();

			static
			gpos::GPOS_RESULT EresUnittest_RunTests();

	}; // class CGinIndexTest
}

#endif // !GPOPT_CGinIndexTest_H

// EOF

