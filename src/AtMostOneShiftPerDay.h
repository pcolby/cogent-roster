#ifndef __AT_MOST_ONE_SHIFT_PER_DAY_H__
#define __AT_MOST_ONE_SHIFT_PER_DAY_H__

#include "ConstraintInterface.h"

namespace Cogent {

/*!
 * \brief Ensures that a nurse does not work more than one shift per day.
 */
class AtMostOneShiftPerDay : public ConstraintInterface
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
        Q_ASSERT(!daysSoFar.isEmpty()); // Days so far includes today, even if empty.
        Q_UNUSED(shift);

        // Remove any nurses that are already rostered today.
        int removedCount = 0;
        foreach (const QVariant &shift, daysSoFar.last().toMap()) {
            foreach (const QVariant &nurse, shift.toList()) {
                if (nurses.remove(nurse.toString())) {
                    removedCount++;
                }
            }
        }
        qDebug() << "removed" << removedCount << "of" << nurses.size()+removedCount << "nurses";
        return removedCount;
    }

};

} // end Cogent namespace

#endif // __AT_MOST_ONE_SHIFT_PER_DAY_H__
