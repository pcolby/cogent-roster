#ifndef __AT_MOST_FIVE_CONSECUTIVE_DAYS_H__
#define __AT_MOST_FIVE_CONSECUTIVE_DAYS_H__

#include "ConstraintInterface.h"

#include <QDebug>

namespace Cogent {

/*!
 * \brief Ensures that a nurse does not work more than five days in a row.
 */
class AtMostFiveConsecutiveDays : public ConstraintInterface
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

        // If we don't have 5 days of history yet, then no need to exclude anyone.
        if (daysSoFar.size() < 6) { // 6 because daysSoFar always include the current day too.
            qDebug() << "removed 0 of" << nurses.size() << "nurses";
            return 0;
        }

        // Find nurses that have been rostered on every day for the past five days.
        QStringSet nursesToRemove = nurses;
        for (int day = daysSoFar.size()-6; day < daysSoFar.size()-1; ++day) {
            QStringSet nursesRosteredThisDay;
            foreach (const QVariant &shift, daysSoFar.at(day).toMap()) {
                foreach (const QVariant &nurse, shift.toList()) {
                    nursesRosteredThisDay.insert(nurse.toString());
                }
            }

            // Remove from nursesToRemove, any nurses that are *not* rostored this day.
            nursesToRemove.subtract(nursesToRemove - nursesRosteredThisDay);
        }
        qDebug() << "removed" << nursesToRemove.size() << "of" << nurses.size() << "nurses";
        nurses.subtract(nursesToRemove);
        return nursesToRemove.size();
    }

};

} // end Cogent namespace

#endif // __AT_MOST_FIVE_CONSECUTIVE_DAYS_H__
