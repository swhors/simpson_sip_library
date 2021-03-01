// 아래의 랜덤 함수는 네이버에서 검색했음.
// 2009/10/28 swhors@naver.com
#include <stdlib.h>

#define MBIG 1000000000
#define MSEED 161803398
#define MZ  0
#define FAC  (1.0/MBIG)

double GetRandomDouble(long idum)
{
	static int inext,inextp;
	static long ma[56];
	static int iff=0;
	long mj, mk;
	int i,ii,k;

	if(idum < 0 || iff ==0)
	{
		iff=1;
		mj=labs(MSEED-labs(idum));
		mj %= MBIG;
		ma[55]=mj;
		mk=1;
		for(i=1;i<=54;i++)
		{
			ii=(21*i)%55;
			ma[ii]=mk;
			mk=mj-mk;
			if(mk<MZ)
				mk+=MBIG;
			mj=ma[ii];
		}
		for(k=1;k<=4;k++)
		{
			for(i=1;i<=55;i++)
			{
				ma[i] -= ma[1+(i+30)%55];
				if(ma[i] < MZ)
					ma[i] += MBIG;
			}
		}
		inextp=0;
		inextp=31;
		idum=1;
	}
	if(++inext == 56)
		inext=1;
	if(++inextp == 56)
		inextp=1;
	mj=ma[inext]-ma[inextp];
	if(mj < MZ)
		mj+=MBIG;
	ma[inext]=mj;
	return mj;
	//return mj*FAC;
}