#ifndef __ROSTER_H__
#define __ROSTER_H__

#include "AtMostFiveConsecutiveDays.h"
#include "AtMostFiveNightShiftsPerMonth.h"
#include "AtMostOneShiftPerDay.h"
#include "NoSingleDaysOff.h"
#include "LeastRecentScheduler.h"

#include <QDate>
#include <QDebug>
#include <QSharedPointer>

namespace Cogent {

class RosterGenerator
{

public:
    RosterGenerator(const int nursesPerShift = 5) :
        constraints{
            QSharedPointer<ConstraintInterface>(new Cogent::AtMostFiveConsecutiveDays()),
            QSharedPointer<ConstraintInterface>(new Cogent::AtMostFiveNightShiftsPerMonth(QObject::tr("night"))),
            QSharedPointer<ConstraintInterface>(new Cogent::AtMostOneShiftPerDay()),
            QSharedPointer<ConstraintInterface>(new Cogent::NoSingleDaysOff()),
        }, scheduler(new LeastRecentScheduler()), nursesPerShift(nursesPerShift)
    { }

    QVariantMap generate(const int year, const int month, const QStringList &nurses)
    {
        const int daysInMonth = RosterGenerator::daysInMonth(year, month);
        static const QStringList shiftNames{
            QObject::tr("morning"), QObject::tr("evening"), QObject::tr("night")
        };

        // A quick sanity check - the harshest constraint is "no more than 5 night shifts per
        // nurse", so the total number of night shifts we can allocate is 5 * the number of nurses
        // available. And the bumber of night shifts slots we need to fill is the number of nights
        // (aka the number of days) * the number of nurses we need on for each shift. These two
        // totals are easy to compare.
        if ((daysInMonth*nursesPerShift) > (nurses.size()*5)) {
            qWarning() << nurses.size() << "nurses is unlikely to be enough for" << daysInMonth << "days";
            // Don't stop though, we'll still give it a try (but expect to fail later).
        }

        QVariantList days;
        while (days.size() < daysInMonth) {
            QVariantMap day;
            foreach (const QString &shift, shiftNames) {
                qDebug() << "day" << days.size()+1 << shift;

                // Build a list of candidate nurses by reducing the full list by each constraint.
                auto candidateNurses = nurses.toSet();
                foreach(auto &constraint, constraints) {
                    constraint->constrain(candidateNurses, shift, days + QVariantList{day});
                }
                qDebug() << "constrained to" << candidateNurses.size() << "of" << nurses.size() << "nurses";

                // Use the scheduler to choose the required number of nurses for this shift.
                QStringList nursesForThisShift;
                while (nursesForThisShift.size() < nursesPerShift) {
                    if (candidateNurses.isEmpty()) {
                        qWarning() << "not enough nurses to satisfy the" << shift
                                   << "shift of day" << days.size()+1;
                        return QVariantMap();
                    }
                    const QString nurse = scheduler->chooseNextNurse(candidateNurses);
                    nursesForThisShift.append(nurse);
                    candidateNurses.remove(nurse);
                }

                // Add this shift to the day.
                day[shift] = nursesForThisShift;
            }
            // This day to the roster.
            days.append(day);
        }

        // Build the final roster, with some metadata.
        QVariantMap roster;
        roster[QObject::tr("%1-%2").arg(year).arg(month,2,10,QLatin1Char('0'))] = days;
        roster[QObject::tr("created")] = QDateTime::currentDateTime().toString();
        // Could add plenty of other metadata here in future.
        return roster;
    }

protected:
    const QVector<QSharedPointer<ConstraintInterface>> constraints;
    const QSharedPointer<SchedulerInterface> scheduler;
    const int nursesPerShift;

    /*!
     * Returns the number of days in the \a month of \a year.
     */
    static int daysInMonth(const int year, const int month)
    {
        const QDate date(year, month, 1); // Every month has a first day (the '1').
        return date.daysInMonth();
    }
};

} // end Cogent namespace

#endif // __ROSTER_H__
