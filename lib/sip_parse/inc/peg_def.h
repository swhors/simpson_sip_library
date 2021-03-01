#pragma once

#include <limits.h>
#include <stdio.h>


namespace peg_templ{

struct C
{
    C():nNofIntegers(0),nNofIdents(0){}
    int nNofIntegers;
    int nNofIdents;
};


template<int CHARVALUE>
struct Char{
    template<typename It,typename Context>
    static bool Matches(It& p,Context* pC)
    {
        return *p==CHARVALUE?(++p,true):false;
    }
};

template<int LOWER,int UPPER,
         int LOWER2=-1,int UPPER2=-1,
         int LOWER3=-1,int UPPER3=-1,
         int LOWER4=-1,int UPPER4=-1>
struct In{
    template<typename It,typename Context>
    static bool Matches(It& p,Context* pC){
        if( *p>=LOWER && *p<=UPPER ){++p;return true;}
        if( LOWER2==-1){return false;}
        if( *p>=LOWER2 && *p<=UPPER2){++p;return true;}
        if( LOWER3==-1){return false;}
        if( *p>=LOWER3 && *p<=UPPER3){++p;return true;}
        if( LOWER4==-1){return false;}
        if( *p>=LOWER4 && *p<=UPPER4){++p;return true;}
        return false;
    }
};

template<int LOWER,int UPPER,typename T>
struct For
{
    enum EMinMaxFor{eMinFor=LOWER,eMaxFor=UPPER};
    template<typename It,typename Context>
    static bool Matches(It& p,Context* pC){
        It pStart=p;
        int i;
        for(i=0;i<=UPPER;i++){
            if( !T::Matches(p,pC) ){
                break;
            }
        }
        return (i>LOWER)?(true):(p=pStart,false);
    }
};

//using namespace std;

template<typename T>
struct FindRepeat
{
    template<typename It,typename Context>
    static bool Matches(It& p,Context* pC){
        It pOr=p;
        bool bOk=false;
        if( p == NULL )
            return false;
        while(!(bOk=T::Matches(p,pC))){
            ++p;
            if( *p <= 0 )
            {
                p = pOr;
                return false;
            }
        }
        if( bOk == false )
            p = pOr;
        return bOk;
    }

    template<typename It,typename Context>
    static bool Matches(It& p,Context* pC,It &be){
        It pOr=p;
        bool bOk=false;
        be = p;
        if( p == NULL )
            return false;
        while(!(bOk=T::Matches(p,pC))){
            ++p;
            be = p;
            if( *p <= 0 )
            {
                p = pOr;
                return false;
            }
        }
        if( bOk == false )
            p = pOr;
        return bOk;
    }
};

template<typename T>
struct Option
{
    template<typename It,typename Context>
    static bool Matches(It& p,Context* pC){
        T::Matches(p,pC);
        return true;
    }
};

template<typename T>
struct OptRepeat
{
    template<typename It,typename Context>
    static bool Matches(It& p,Context* pC){
        while(T::Matches(p,pC)){
        }
        return true;
    }
};

template<int LOWER,int UPPER,typename T>
struct ForCheck
{
    template<typename It,typename Context>
    static bool Matches(It& p, Context* pC)
    {
        It pOr;
        int pos=0;
        pOr = p;
        if(!T::Matches(p,pC))
        {
            if( LOWER == 0)
            {
                return true;
            }
            p = pOr;
            return false;
        }
        pos++;
        while(T::Matches(p,pC))
        {
            pos++;
        }
        if( pos < LOWER && pos > UPPER )
        {
            p=pOr;
            return false;
        }
        return true;
    }
};

template<typename T>
struct PlusRepeat
{
    template<typename It,typename Context>
    static bool Matches(It& p, Context* pC){
        if(!T::Matches(p,pC)){
            return false;
        }
        while(T::Matches(p,pC)){
        }
        return true;
    }
};

struct MatchTrue{
    enum EMinMax{eMinChar=INT_MAX,eMaxChar=INT_MIN};
    template<typename It,typename Context>
    static bool Matches(It& p,Context* pC){return true;}
};

struct MatchFalse{
    enum EMinMax{eMinChar=INT_MAX,eMaxChar=INT_MIN};
    template<typename It,typename Context>
    static bool Matches(It& p,Context* pC){return false;}
};

template<class T0,class T1,
            class T2=MatchFalse,class T3=MatchFalse,class T4=MatchFalse,
            class T5=MatchFalse,class T6=MatchFalse,class T7=MatchFalse,
            class T8=MatchFalse,class T9=MatchFalse,class T10=MatchFalse,
            class T11=MatchFalse,class T12=MatchFalse,class T13=MatchFalse,
            class T14=MatchFalse,class T15=MatchFalse,class T16=MatchFalse,
            class T17=MatchFalse,class T18=MatchFalse,class T19=MatchFalse,
            class T20=MatchFalse,class T21=MatchFalse,class T22=MatchFalse,
            class T23=MatchFalse,class T24=MatchFalse,class T25=MatchFalse>
struct Or
{
    template<typename It,typename Context>
    static bool Matches(It& p,Context* pC){
        return T0::Matches(p,pC)||T1::Matches(p,pC)
            || T2::Matches(p,pC)||T3::Matches(p,pC)
            || T4::Matches(p,pC)||T5::Matches(p,pC)
            || T6::Matches(p,pC)||T7::Matches(p,pC)
            || T8::Matches(p,pC)||T9::Matches(p,pC)
            || T10::Matches(p,pC)||T11::Matches(p,pC)
            || T12::Matches(p,pC)||T13::Matches(p,pC)
            || T14::Matches(p,pC)||T15::Matches(p,pC)
            || T16::Matches(p,pC)||T17::Matches(p,pC)
            || T18::Matches(p,pC)||T19::Matches(p,pC)
            || T20::Matches(p,pC)||T21::Matches(p,pC)
            || T22::Matches(p,pC)||T23::Matches(p,pC)
            || T24::Matches(p,pC)||T25::Matches(p,pC);
    }
};

template<int C0,int C1,int C2=INT_MIN,int C3=INT_MIN,int C4=INT_MIN,
         int C5=INT_MIN,int C6=INT_MIN,int C7=INT_MIN>
struct OneOfChars
{
    template<typename It,typename Context>
    static bool Matches(It& p,Context* pC)
    {
        if( *p==C0||*p==C1){++p;return true;}
        if( C2==INT_MIN){return false;}
        if( *p==C2){++p;return true;}
        if( C3==INT_MIN){return false;}
        if( *p==C3){++p;return true;}
        if( C4==INT_MIN){return false;}
        if( *p==C4){++p;return true;}
        if( C5==INT_MIN){return false;}
        if( *p==C5){++p;return true;}
        if( C6==INT_MIN){return false;}
        if( *p==C6){++p;return true;}
        if( C7==INT_MIN){return false;}
        if( *p==C7){++p;return true;}
        return false;
    }
};

template<typename T0, typename T1,
		typename T2=MatchTrue, typename T3=MatchTrue,
		typename T4=MatchTrue, typename T5=MatchTrue,
		typename T6=MatchTrue, typename T7=MatchTrue,
		typename T8=MatchTrue, typename T9=MatchTrue,
		typename T10=MatchTrue, typename T11=MatchTrue,
		typename T12=MatchTrue, typename T13=MatchTrue,
		typename T14=MatchTrue, typename T15=MatchTrue,
		typename T16=MatchTrue, typename T17=MatchTrue>
struct And
{
    template<typename It, typename Context>
    static bool Matches(It& p, Context* pC)
    {
        It pStart=p;
        if( T0::Matches(p,pC)&&T1::Matches(p,pC)
          &&T2::Matches(p,pC)&&T3::Matches(p,pC)
          &&T4::Matches(p,pC)&&T5::Matches(p,pC)
          &&T6::Matches(p,pC)&&T7::Matches(p,pC)
          &&T8::Matches(p,pC)&&T9::Matches(p,pC)
          &&T10::Matches(p,pC)&&T11::Matches(p,pC)
          &&T12::Matches(p,pC)&&T13::Matches(p,pC)
          &&T14::Matches(p,pC)&&T15::Matches(p,pC)
          &&T16::Matches(p,pC)&&T17::Matches(p,pC)){
            return true;
        } else {
            p=pStart;
            return false;
        }
    }
};

template <int C0, int C1=INT_MIN,int C2=INT_MIN,int C3=INT_MIN,
          int C4=INT_MIN,int C5=INT_MIN,int C6=INT_MIN,int C7=INT_MIN>
struct String
{
    template<typename It,typename Context>
    static bool Matches(It&p,Context* pC)
    {
        if(*(*p==C0)){return false;}
        It pStart = p;
        ++p;if(C1==INT_MIN) {return true;}
        if(!(*p==C1))       {p=pStart;return false;}
        ++p;if(C2==INT_MIN) {return true;}
        if(!(*p==C2))       {p=pStart;return false;}
        ++p;if(C3==INT_MIN) {return true;}
        if(!(*p==C3))       {p=pStart;return false;}
        ++p;if(C4==INT_MIN) {return true;}
        if(!(*p==C4))       {p=pStart;return false;}
        ++p;if(C5==INT_MIN) {return true;}
        if(!(*p==C5))       {p=pStart;return false;}
        ++p;if(C6==INT_MIN) {return true;}
        if(!(*p==C6))       {p=pStart;return false;}
        ++p;if(C7==INT_MIN) {return true;}
        if(!(*p==C7))       {p=pStart;return false;}
        ++p; return true;
    }
};

template<typename T>
struct Not{
    template<typename It,typename Context>
    static bool Matches(It& p,Context* pC)
    {
        It p0=p;
        return !T::Matches(p0,pC);
    }
};

template<typename T>
struct Peek{
    template<typename It,typename Context>
    static bool Matches(It& p,Context* pC)
    {
        It p0=p;
        return T::Matches(p0,pC);
    }
};

} // namespace peg_templ

