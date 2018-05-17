#ifndef __AT_MOST_FIVE_NIGHT_SHIFTS_PER_MONTH_H__
#define __AT_MOST_FIVE_NIGHT_SHIFTS_PER_MONTH_H__

#include "ConstraintInterface.h"

namespace Cogent {

/*!
 * \brief Ensures that a nurse does not work more than five night shifts per month.
 */
class AtMostFiveShiftsPerMonth : public ConstraintInterface
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
        Q_UNUSED(nurses);
        Q_UNUSED(shift);
        Q_UNUSED(daysSoFar);
        return 0;

    }

};

} // end Cogent namespace

#endif // __AT_MOST_FIVE_NIGHT_SHIFTS_PER_MONTH_H__
