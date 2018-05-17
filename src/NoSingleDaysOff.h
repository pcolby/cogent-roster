#ifndef __NO_SINGLE_DAYS_OFF_H__
#define __NO_SINGLE_DAYS_OFF_H__

#include "ConstraintInterface.h"

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
        Q_UNUSED(nurses);
        Q_UNUSED(shift);
        Q_UNUSED(daysSoFar);
        return 0;

    }

};

} // end Cogent namespace

#endif // __NO_SINGLE_DAYS_OFF_H__
