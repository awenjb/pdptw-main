#include "time_window.h"

TimeWindow::TimeWindow(TimeInteger s, TimeInteger e) : start(s), end(e) {}

TimeInteger TimeWindow::getStart() const
{
    return start;
}

TimeInteger TimeWindow::getEnd() const
{
    return end;
}

TimeInteger TimeWindow::getWidth() const
{
    return std::max<TimeInteger>(0, getEnd() - getStart());
}

bool TimeWindow::isValid(TimeInteger t) const
{
    return t <= getEnd();
}

bool TimeWindow::isIn(TimeInteger t) const
{
    return start <= t && t <= end;
}

void TimeWindow::print() const
{
    std::cout << "Time Window : [" << start << ", " << end << "] \n";
}
