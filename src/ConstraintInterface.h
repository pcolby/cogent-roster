#ifndef __CONSTRAINT_INTERFACE_H__
#define __CONSTRAINT_INTERFACE_H__

#include <QSet>
#include <QString>
#include <QVariantList>

namespace Cogent {

/*!
 * \brief Interface class for all constraint classes to implement.
 */
class ConstraintInterface
{

public:
    typedef QSet<QString> QStringSet;

    /*!
     * \brief Removes from \a nurses any nurse that would fail this constraint if thery were to be
     * included in \a shift of the day after \a daysSoFar.
     *
     * Returns the number of nurses removed, if any, otherwise 0.
     */
    virtual int constrain(QStringSet &nurses, const QString &shift, const QVariantList &daysSoFar) = 0;

    /*!
     * \brief Virtual destructor for safe polymorphic destruction.
     */
    virtual ~ConstraintInterface() { }
};

} // end Cogent namespace

#endif // __CONSTRAINT_INTERFACE_H__
