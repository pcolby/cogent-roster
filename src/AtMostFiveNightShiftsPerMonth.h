#ifndef __AT_MOST_FIVE_NIGHT_SHIFTS_PER_MONTH_H__
#define __AT_MOST_FIVE_NIGHT_SHIFTS_PER_MONTH_H__

#include "ConstraintInterface.h"

namespace Cogent {

/*!
 * \brief Ensures that a nurse does not work more than five night shifts per month.
 */
class AtMostFiveNightShiftsPerMonth : public ConstraintInterface
{

public:
    AtMostFiveNightShiftsPerMonth(const QString &nightShiftLabel): nightShiftLabel(nightShiftLabel) { }

    /*!
     * \brief Removes from \a nurses any nurse that would fail this constraint if thery were to be
     * included in \a shift of the day after \a daysSoFar.
     *
     * Returns the number of nurses removed, if any, otherwise 0.
     */
    int constrain(QStringSet &nurses, const QString &shift, const QVariantList &daysSoFar) override
    {
        // This constraint does not apply to non-night-shifts.
        if (shift != nightShiftLabel) {
            qDebug() << "removed 0 of" << nurses.size() << "nurses";
            return 0;
        }

        // Count how many times each of the interested nurses are rostered for night shifts already.
        QMap<QString, int> nightShiftsPerNurse;
        foreach (const QVariant &day, daysSoFar) {
            foreach (const QVariant &nurse, day.toMap().value(nightShiftLabel).toList()) {
                if (nurses.contains(nurse.toString())) {
                    nightShiftsPerNurse[nurse.toString()]++;
                }
            }
        }

        // Remove all nurses that have five (or more) night shifts already.
        int removedCount = 0;
        foreach (const QString &nurse, nurses) {
            const int nightShiftsCount = nightShiftsPerNurse.value(nurse);
            if (nightShiftsCount >= 5) {
                if (nightShiftsCount > 5) {
                    qWarning() << "roster already violates AtMostFiveShiftsPerMonth constraint for"
                               << nurse << "with" << nightShiftsCount << "night shifts";
                }
                nurses.remove(nurse);
                removedCount++;
            }
        }
        qDebug() << "removed" << removedCount << "of" << nurses.size()+removedCount << "nurses";
        return removedCount;
    }

protected:
    const QString nightShiftLabel;

};

} // end Cogent namespace

#endif // __AT_MOST_FIVE_NIGHT_SHIFTS_PER_MONTH_H__
