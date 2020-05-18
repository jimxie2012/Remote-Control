#include <math.h>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <algorithm> // For swap()
#include <ctime>
#include <cassert>
#include <iomanip>
#include <time.h>
#include "SHTime.h"
#include <string.h>

//const char comment = '#';
/// Because this function has no validity checks, you should make sure that these parameters are valid.
/// I only use this function on Sun Solaris platform, because the gcc 3.4 function 'mktime' on which
/// seems not work properly.
time_t MakeTime(int yr, int mon, int day, int hr, int min, int sec)
{
    struct tm t_tm;
    time_t now;  
    time(&now);
    //取出系统实际 tm_isdst值
#ifdef __GNUC__
    localtime_r(&now, &t_tm);  
#else
    struct   tm   *tmNow = localtime(&now);
    t_tm.tm_isdst = tmNow->tm_isdst;
#endif  

    t_tm.tm_sec=sec;
    t_tm.tm_min=min;
    t_tm.tm_hour=hr;
    t_tm.tm_mday=day;
    t_tm.tm_mon=mon;
    t_tm.tm_year=yr;
    //此处应该使用系统实际的值，不能假设就是0
    //t_tm.tm_isdst=0;

    return(mktime(&t_tm));
}

namespace 
{
    const int daysInMonth[][13] = 
    {
        { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
    };
    inline bool isleap(int y) {
        return y%4 == 0 && y%100 != 0 || y%400 == 0;
    }
}
/*
CSHTime(int )
{


}
*/

CSHTime::CSHTime() 
{
    /// Get current date
    time_t tval = time(0);
    
    /// Commented at 20070808
        ///struct tm *now = localtime(&tval);
    
    /// End of commenting - 20070808
    
    /// Added at 20070808
    #ifdef __GNUC__
        struct tm *now = (struct tm *)malloc( sizeof( struct tm ) );
        localtime_r( &tval, now );
        
    #else
        struct tm *now = localtime(&tval);
    
    #endif  /// End of #ifdef __GNUC__
    
    /// End of adding - 20070808
    
    year = now->tm_year + 1900;
    month = now->tm_mon + 1;
    day = now->tm_mday;
    hour = now->tm_hour;
    minute = now->tm_min;
    second = now->tm_sec;
    
    /// Added at 20070808
    #ifdef __GNUC__
        free( now );

    #endif  /// End of #ifdef __GNUC__
    
    /// End of adding - 20070808
}

CSHTime::CSHTime(int yr,int mon,int dy,int hr,int min,int sec) throw(CSHTime::DateError) 
{
    if (!(1 <= mon && mon <= 12))
        throw DateError("Bad month in Date ctor");
    if (!(1 <= dy && dy <= daysInMonth[isleap(year)][mon]))
        throw DateError("Bad day in Date ctor");
    if (!(0 <= hr && hr <= 23))
        throw DateError("Bad hour in Date ctor");
    if (!(0 <= min && min <= 59))
        throw DateError("Bad minute in Date ctor");
    if (!(0 <= sec && sec <= 59))
        throw DateError("Bad second in Date ctor");
    year = yr;
    month = mon;
    day = dy;
    hour = hr;
    minute = min;
    second = sec;
}
 
CSHTime::CSHTime(int yr, int mon, int dy) throw(CSHTime::DateError) 
{
    if (!(1 <= mon && mon <= 12))
        throw DateError("Bad month in Date ctor");
    if (!(1 <= dy && dy <= daysInMonth[isleap(year)][mon]))
        throw DateError("Bad day in Date ctor");
    year = yr;
    month = mon;
    day = dy;
    hour = 0;
    minute = 0;
    second = 0;
}
 
CSHTime::CSHTime(const std::string& s) throw(CSHTime::DateError) 
{
    /// Assumes YYYYMMDDHHMISS format
    if (!(s.size() == 14))
        throw DateError("Bad string in Date ctor");
    for (int n = 14; --n >= 0;)
        if (!isdigit(s[n]))
            throw DateError("Bad string in Date ctor");
    year = atoi(s.substr(0, 4).c_str());
    month = atoi(s.substr(4, 2).c_str());
    day = atoi(s.substr(6, 2).c_str());
    hour = atoi(s.substr(8,2).c_str());
    minute = atoi(s.substr(10,2).c_str());
    second = atoi(s.substr(12,2).c_str());
   
    if (!(1 <= month && month <= 12))
        throw DateError("Bad month in Date ctor");
    if (!(1 <= day && day <= daysInMonth[isleap(year)][month]))
        throw DateError("Bad day in Date ctor");
    if (!(0 <= hour && hour <= 23))
        throw DateError("Bad hour in Date ctor");
    if (!(0 <= minute && minute <= 59))
        throw DateError("Bad minute in Date ctor");
    if (!(0 <= second && second <= 59))
        throw DateError("Bad second in Date ctor");
}

#ifdef __GNUC__
 CSHTime::CSHTime(const std::string& time, const std::string& format)throw(CSHTime::DateError)
 {
    struct tm temptime;

    if ( !strptime(time.c_str(), format.c_str(), &temptime))
    {
        throw DateError("Bad time with this format");
    }

    year = temptime.tm_year + 1900;
    month = temptime.tm_mon + 1;
    day = temptime.tm_mday;
    hour = temptime.tm_hour;
    minute = temptime.tm_min;
    second = temptime.tm_sec;
 }
#endif
 
int32 CSHTime::getYear() const { return year; }
 
int32 CSHTime::getMonth() const { return month; }
 
int32 CSHTime::getDay() const { return day; }

int32 CSHTime::getHour() const { return hour; }

int32 CSHTime::getMinute() const { return minute; }

int32 CSHTime::getSecond() const { return second; }
 
string CSHTime::toString(string theFrmString)
{
    string ret = "";
    
    struct tm *aTm = new struct tm;
    aTm->tm_year = year-1900;
    aTm->tm_mon = month-1;
    aTm->tm_mday = day;
    aTm->tm_hour = hour;
    aTm->tm_min = minute;
    aTm->tm_sec = second;

    char buffer[200];
    memset(buffer,0,200);
    strftime(buffer,sizeof(buffer),theFrmString.c_str(),aTm);
    ret = buffer;
    delete aTm;
    return ret;
}
 int32 CSHTime::getWeekDay(int nYear, int nMonth, int nDay)
 {
    /// Modified at 20070808
        /*
        int i,j,count=0;
        int Day_Index;
        */
    
        int j,count=0;
        
    /// End of modifing - 20070808
     
     //int days[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
     int MonthAdd[12] = {0,31,59,90,120,151,181,212,243,273,304,334};

    count = MonthAdd[nMonth-1];

    count = count + nDay;
    if((nYear%4 == 0&&nYear %100 != 0||nYear % 400 == 0)&& nMonth >= 3)
       count += 1;
    count = count + (nYear - 1901) * 365;
    for(j = 1901;j < nYear;j++)
    {
        if(j % 4 == 0 && j % 100 != 0 || j % 400 == 0)
            count++;
    }
    
    if(((count+1) % 7)==0)
        return 7;
    else
        return ((count+1) % 7);	
 }

int CSHTime::compare(const CSHTime& d2) const 
{
    int result = year - d2.year;
    if (result == 0) 
    {
        result = month - d2.month;
        if (result == 0)
        {
            result = day - d2.day;
            if (result == 0)
            {
                result = hour - d2.hour;
                if (result == 0)
                {
                    result = minute - d2.minute;
                    if (result == 0)
                        result = second - d2.second;
                }
            }
        }
    }
    return result;
}
 
int CSHTime::daysInPrevMonth(int year, int month) 
{
    if(month == 1) 
    {
        --year;
        month = 12;
    }
    else
        --month;
    return daysInMonth[isleap(year)][month];
}

CSHTime& CSHTime::operator=(const CSHTime& d)
{
    year = d.year;
    month = d.month;
    day = d.day;
    hour = d.hour;
    minute = d.minute;
    second = d.second;
    return *this;
}

CSHTime& CSHTime::operator-=(const int seconds)
{
    time_t tt = MakeTime(year-1900,month-1,day,hour,minute,second);
    tt -= seconds;
    struct tm tstm;
    
    /// Commented at 20070808
        ///tstm = *localtime(&tt);
    
    /// End of commenting - 20070808
    
    /// Added at 20070808
    #ifdef __GNUC__
        localtime_r( &tt, &tstm );
        
    #else
        tstm = *localtime(&tt);
    
    #endif  /// End of #ifdef __GNUC__
    
    /// End of adding - 20070808
    
    year = tstm.tm_year+1900;
    month = tstm.tm_mon+1;
    day = tstm.tm_mday;
    hour = tstm.tm_hour;
    minute = tstm.tm_min;
    second = tstm.tm_sec;
    return *this;
}

CSHTime& CSHTime::operator+=(const int seconds)
{

    time_t tt = MakeTime(year-1900,month-1,day,hour,minute,second);
    tt += seconds;

    struct tm tstm;
    
    /// Commented at 20070808
        ///tstm = *localtime(&tt);
    
    /// End of commenting - 20070808
    
    /// Added at 20070808
    #ifdef __GNUC__
        localtime_r( &tt, &tstm );
        
    #else
        tstm = *localtime(&tt);
    
    #endif  /// End of #ifdef __GNUC__
    
    /// End of adding - 20070808

    year = tstm.tm_year+1900;
    month = tstm.tm_mon+1;
    day = tstm.tm_mday;
    hour = tstm.tm_hour;
    minute = tstm.tm_min;
    second = tstm.tm_sec;
    return *this;
}

CSHTime CSHTime::operator-(const int seconds)
{
    CSHTime ret;
    time_t tt = MakeTime(year-1900,month-1,day,hour,minute,second);
    tt -= seconds;
    struct tm tstm;
    
    /// Commented at 20070808
        ///tstm = *localtime(&tt);
    
    /// End of commenting - 20070808
    
    /// Added at 20070808
    #ifdef __GNUC__
        localtime_r( &tt, &tstm );
        
    #else
        tstm = *localtime(&tt);
    
    #endif  /// End of #ifdef __GNUC__
    
    /// End of adding - 20070808
    
    ret.year = tstm.tm_year+1900;
    ret.month = tstm.tm_mon+1;
    ret.day = tstm.tm_mday;
    ret.hour = tstm.tm_hour;
    ret.minute = tstm.tm_min;
    ret.second = tstm.tm_sec;
    return ret;
}

CSHTime CSHTime::operator+(const int seconds)
{
    CSHTime ret;
    time_t tt = MakeTime(year-1900,month-1,day,hour,minute,second);
    tt += seconds;
    struct tm tstm;
    
    /// Commented at 20070808
        ///tstm = *localtime(&tt);
    
    /// End of commenting - 20070808
    
    /// Added at 20070808
    #ifdef __GNUC__
        localtime_r( &tt, &tstm );
        
    #else
        tstm = *localtime(&tt);
    
    #endif  /// End of #ifdef __GNUC__
    
    /// End of adding - 20070808
    
    ret.year = tstm.tm_year+1900;
    ret.month = tstm.tm_mon+1;
    ret.day = tstm.tm_mday;
    ret.hour = tstm.tm_hour;
    ret.minute = tstm.tm_min;
    ret.second = tstm.tm_sec;
    return ret;
}
 
bool operator<(const CSHTime& d1, const CSHTime& d2) 
{
    return d1.compare(d2) < 0;
}
bool operator<=(const CSHTime& d1, const CSHTime& d2) 
{
    return d1 < d2 || d1 == d2;
}
bool operator>(const CSHTime& d1, const CSHTime& d2) 
{
    return !(d1 < d2) && !(d1 == d2);
}

bool operator>=(const CSHTime& d1, const CSHTime& d2) 
{
    return !(d1 < d2);
}

bool operator==(const CSHTime& d1, const CSHTime& d2) 
{
    return d1.compare(d2) == 0;
}
bool operator!=(const CSHTime& d1, const CSHTime& d2) 
{
    return !(d1 == d2);
}
 
int duration(const CSHTime& date1, const CSHTime& date2) 
{
 
    time_t t1,t2;  
    t1 = MakeTime(date1.year-1900,date1.month-1,date1.day,
                  date1.hour,date1.minute,date1.second);
    t2 = MakeTime(date2.year-1900,date2.month-1,date2.day,
                  date2.hour,date2.minute,date2.second);
  
    /// Modified at 20070808
        ///return t1-t2;
    
        return( (int)( t1 - t2 ) );
    /// End of modifing - 20070808
}


int operator-(const CSHTime& d1, const CSHTime& d2)
{
    return duration(d1, d2); 
}

 
std::ostream& operator<<(std::ostream& os, const CSHTime& d) 
{
    os << d.getYear() << '-'
    << d.getMonth() << '-'
    << d.getDay() << ' '
    << d.getHour() << ':'
    << d.getMinute() << ':'
    << d.getSecond();
    return os;
}
 
std::istream& operator>>(std::istream& is, CSHTime& d) 
{
	/*
    is >> d.year;
    char dash;
    is >> dash;
    if(dash != '-')
        is.setstate(std::ios::failbit);
    is >> d.month;
    is >> dash;
    if(dash != '-')
        is.setstate(std::ios::failbit);
    is >> d.day;

    is >> dash;
    if (dash != ' ')
        is.setstate(std::ios::failbit);
    is >> d.hour;

    is >> dash;
    if (dash != ':')
        is.setstate(std::ios::failbit);
    is >> d.minute;

    is >> dash;
    if (dash != ':')
        is.setstate(std::ios::failbit);
    is >> d.second;
*/
    return is;
} ///:~

///#############################################################################
/// END
