#pragma once
#include "Common.h"
#include <stdexcept>
#include <iosfwd>

class SH_EXPORT CSHTime
{
private:
    int32 year, month, day, hour, minute, second;
    
    static int32 daysInPrevMonth(int32 year, int32 mon);
public:
    /// An exception class
    struct DateError : public std::logic_error 
    {
        DateError(const std::string& msg = ""): std::logic_error(msg) {}
    };    
    CSHTime();
    /// CSHTime(int );
    CSHTime(int, int, int, int, int, int) throw(DateError);
    CSHTime(int, int, int) throw(DateError);
    CSHTime(const std::string&) throw(DateError);
    int32 compare(const CSHTime&) const;
    int32 getYear() const;
    int32 getMonth() const;
    int32 getDay() const;
    int32 getHour() const;
    int32 getMinute() const;
    int32 getSecond() const;
    string toString(string theFrmString="%Y-%m-%d %H:%M:%S");
    int32 getWeekDay(int nYear, int nMonth, int nDay);
    #ifdef __GNUC__
    CSHTime(const std::string&, const std::string&)throw(DateError);
    #endif
    CSHTime& operator=(const CSHTime& d);
    CSHTime& operator-=(const int seconds);
    CSHTime& operator+=(const int seconds);
    CSHTime operator-(const int seconds);
    CSHTime operator+(const int seconds);

    /// Represents as seconds
    friend  int  duration(const CSHTime&, const CSHTime&);
    friend  int  operator-(const CSHTime&, const CSHTime&);
    friend  bool operator<(const CSHTime&, const CSHTime&);
    friend  bool operator<=(const CSHTime&, const CSHTime&);
    friend  bool operator>(const CSHTime&, const CSHTime&);
    friend  bool operator>=(const CSHTime&, const CSHTime&);
    friend  bool operator==(const CSHTime&, const CSHTime&);
    friend  bool operator!=(const CSHTime&, const CSHTime&);
    friend  std::ostream& operator<<(std::ostream&,const CSHTime&);
    friend  std::istream& operator>>(std::istream&, CSHTime&);
};
