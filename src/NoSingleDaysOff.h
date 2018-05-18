#ifndef __NO_SINGLE_DAYS_OFF_H__
#define __NO_SINGLE_DAYS_OFF_H__

#include "ConstraintInterface.h"

#include <QDebug>

namespace Cogent {

/*!
 * \brief Ensures that a nurse's days off occur in groups of two or more.
 *
 * Note, a nurse could still have just one day off a month bounday. This can be covered quite easily
 * if we supported loading previous-month data (ot least the last day).
 */
class NoSingleDaysOff : public ConstraintInterface
{

public:
    /*!
     * \brief Removes from \a nurses any nurse that would fail this constraint if thery were to be
     * included in \a shift of the day after \a daysSoFar.
     *
     * Returns the number of nurses removed, if any, otherwise 0.
     */
    int constrain(QStringSet &nurses, const QString &shift, const QVariantList &daysSoFar) override
    {
        Q_UNUSED(shift);

        // If we don't have 1 day of history yet, then no need to exclude anyone.
        if (daysSoFar.size() < 2) { // 2 because daysSoFar always include the current day too.
            qDebug() << "removed 0 of" << nurses.size() << "nurses";
            return 0;
        }

        // Find which nurses had yesterday off.
        QStringSet nursesWithYesterdayOff = nurses;
        foreach (const QVariant &shift, daysSoFar.at(daysSoFar.size()-2).toMap()) {
            foreach (const QVariant &nurse, shift.toList()) {
                nursesWithYesterdayOff.remove(nurse.toString());
            }
        }
        qDebug() << nursesWithYesterdayOff.size() << "of" << nurses.size() << "nurses had yesterday off";

        // Find which nurses had (at least) two days off.
        QStringSet nursesWithLastTwoDaysOff = nursesWithYesterdayOff;
        if (daysSoFar.size() > 2) { // If we had two (or more) days of history.
            foreach (const QVariant &shift, daysSoFar.at(daysSoFar.size()-3).toMap()) {
                foreach (const QVariant &nurse, shift.toList()) {
                    nursesWithLastTwoDaysOff.remove(nurse.toString());
                }
            }
        }
        qDebug() << nursesWithLastTwoDaysOff.size() << "of" << nurses.size() << "nurses had two days off";

        // Remove all nurses with yesterday off, but not two (or more) days off yet (they need to
        // have today off to allow for their days off to be grouped in two or more days).
        const int originalSize = nurses.size();
        nurses.subtract(nursesWithYesterdayOff - nursesWithLastTwoDaysOff);
        qDebug() << "removed" << (originalSize-nurses.size()) << "of" << originalSize << "nurses";
        return originalSize-nurses.size();
    }

};

} // end Cogent namespace

#endif // __NO_SINGLE_DAYS_OFF_H__
