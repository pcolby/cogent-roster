#ifndef __LEAST_RECENT_SCHEDULER_H__
#define __LEAST_RECENT_SCHEDULER_H__

#include "SchedulerInterface.h"


namespace Cogent {

/*!
 * \brief Interface class for all schedulers to implement.
 */
class LeastRecentScheduler : public SchedulerInterface
{

public:

    /*!
     * \brief Returns the next nurse to fill a roster position given a list of \a availableNurses.
     *
     * This implementation returns the nurse that has least-recently been rostered on. In the case
     *i of a tie (for equally least-recent), any one of the tied nurses will be returned.
     *
     * Note for Cogent: We could provide a guarantee here re the tie behaviour, but there's no need
     * doing so unnecessarily limits our ability to optimise the code to meet other businees needs
     * should they arise.
     *
     * This implementation is O(n).
     */
    virtual QString chooseNextNurse(const QStringSet &availableNurses) override
    {
        Q_ASSERT(!availableNurses.isEmpty()); // Must have at least one nurse available.

        // First check if there's any available nurses never seen before; if so, we are ffee to
        // return any one of them.
        const QStringSet unseenNurses =
            QStringSet(availableNurses).subtract(QStringSet::fromList(allocationHistory));
        if (!unseenNurses.isEmpty()) {
            const QString nurse = *unseenNurses.constBegin();
            qDebug() << "Chose previously-unseen nurse" << nurse;
            allocationHistory.append(nurse);
            return nurse;
        }

        // Since unseenNurses was empty (else we would have returned above), availableNurses is a
        // subset of allocationHistory (ie allocationHistory wholy contains availableNurses).
        Q_ASSERT(allocationHistory.toSet().contains(availableNurses));

        // Further, since availableNurses is non-empty, an allocationHistory is a superset, then
        // allocationHistory cannot now be empty.
        Q_ASSERT(!allocationHistory.isEmpty());

        // Return the first (oldest allocated) nurse from the allocation history that is also in the
        // available nurses list. This is guaranteed to find, and thus return, a valid nurse, based
        // on the above two assertions.
        foreach (const QString &nurse, allocationHistory)
        {
            if (availableNurses.contains(nurse)) {
                qDebug() << "Chose previously-seen nurse" << nurse;
                const int removeCount = allocationHistory.removeAll(nurse);
                Q_ASSERT(removeCount == 1); // We maintain unique nurse entries in the history.
                allocationHistory.append(nurse);
                return nurse;
            }
        }

        Q_ASSERT(false); // Arriving here should be impossible (see the assertions above).
        return QString();
    }

protected:
    QStringList allocationHistory; // From oldest allocated, to most recent.

};

} // end Cogent namespace

#endif // __LEAST_RECENT_SCHEDULER_H__
