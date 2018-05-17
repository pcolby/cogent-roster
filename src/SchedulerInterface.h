#ifndef __SCHEDULER_INTERFACE_H__
#define __SCHEDULER_INTERFACE_H__

#include <QSet>
#include <QString>

namespace Cogent {

/*!
 * \brief Interface class for all schedulers to implement.
 */
class SchedulerInterface
{

public:
    typedef QSet<QString> QStringSet;

    /*!
     * \brief Returns the next nurse to fill a roster position given a list of \a availableNurses.
     *
     * Derived classed must implement this virtual function to apply their own scheduling
     * heuristic.
     */
    virtual QString chooseNextNurse(const QStringSet &availableNurses) = 0;

    /*!
     * \brief Virtual destructor for safe polymorphic destruction.
     */
    virtual ~SchedulerInterface() { }
};

} // end Cogent namespace

#endif // __SCHEDULER_INTERFACE_H__
