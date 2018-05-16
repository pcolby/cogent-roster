# Nurse Rostering

## Analysis

Let's start with some basic analysis of the problem.

### Duplicate Nurses

First off, there's a glut of nurses in the sample file, but actually how many
nurses are present depends on how we treat duplicates. eg:

```
paul@paul-XPS-13-9343:~/src/cogent$ cat nurses.txt | wc -l
109
paul@paul-XPS-13-9343:~/src/cogent$ cat nurses.txt | sort -u | wc -l
98
```

Note that `nurses.txt` actually has 110 lines, but `wc` doesn't count the last
line, since it has no trailing newline character.

So, there are 12 nurse names that are duplicated; they are:

```
paul@paul-XPS-13-9343:~/src/cogent$ cat nurses.txt | sort | uniq -c | sort -n | grep -v '^\s*1 '
      2 Gabi
      2 Gael
      2 Gal
      2 Gale
      2 Galen
      2 Galway
      2 Gannet
      2 Garcia
      2 Gardener
      2 Gardner
      2 Garnet
      2 Gary
```

These duplicates could be valid, differnt nurses with the same name, or errors
in the file. In the former case, we would actually need unique IDs of some
kind, and in the latter, we need to ignore dupliates. Of course, in a real
world scenario, this would be a question for the customer wanting the product,
but since this is a coding test, we'll simply support both options. In the case
of needing unique IDs, we'll simply use line numbers.

### How Many Nurses Does It Take To Fill A Roster?

Its worth noting, that with so many nurses (98 if we exclude duplicates), we
can use a very naive least-recently-used (LRU) scheduling algorithm. This is
because:

* the maximum number of slots in a month is 465 (that is 31 days * 3 shifts * 5
  nurses per shift)
* an simple LRU algorithm would spread each nurses shifts as far apart as
  possible
* each nurse would have only 4 or 5 shifts for this entire month (the mean is
  4.74 shifts per nurse per month)
* if 4 or 5 shifts is spread uniformly over the month, they can't possibly
  violate any of the stated Considerations

So, the right thing to do here, would be to clarify the situation with the
customer - ie does the customer guarantee that they always have so many if
nurses available?  If so, we can produce a much quicker, simpler solution.

However, again, since this is a coding exercise, let's assume that such a
guarantee cannot be made.

Even as we reduce the pool of available nurses, the same LRU algorithm would be
optimal, if it weren't for one Consideration: the need to group days off into
at least pairs.  Without this Consideration, spreading the available nurses as
efficiently as possible would work fine. It should be pretty easy to overlay
the at-least-two-dayas-off-at-a-time constraint over an LRU algorithm though.
Will come back to that later.

So, how many nurses do we need? (Not necessarily important to know for the
solution, but might help us to understand the problem space a little more).

We need at least 15 unique nurses to prevent any working two shifts per day
(Considerations 1 and 2). Those same 15 nurses can do 5 out of every 7 days
(due to Constraints 3 and 5), and can do so for the entire month without
voiolating the five-nights (Consideration 4). The remaining 2 of every 7 days
can be covered by another 15 nurses - but not less. So the minimum number of
nurses required is 30, with some being used more or less efficiently than
others.

This will be interesting to test.

All of this assumes that we're not allowing other, undocumented constraints,
such as nurses that can or can't work certain days, or wants to or can't work
with specific other nurses (or doctors). These features would be common in a
medical practice management system, but outside the scope of the current
solution.

Having said that, I'm leaning towards a simple algorithm that first applies
a constraints-based reduction of available nurses per slot, then selects from
the remaining nurses with a simple LRU scheduling heuristic. (I refer to
"scheduling" here in the computing sense, not the business sense, ie
https://en.wikipedia.org/wiki/Scheduling_(computing)). By separating the logic
into constraints and scheduling, we can add further constraints quite easily,
and even swap out the scheduling algorithm (eg we might want cheapest nurses,
or nurses with the least number of complaints, etc).

The disadvantage to this approach, is simply that if the number of constraints
became substantial (specifically the must / can't work specific dates type)
then the proposed algorithm may make early choices that prevent it being to
satisfy later constraints. In this case, the algorithm would need to be to
begin with those temporal constraints and work backward to reach a solution in
a meaningful timeframe.

Also worth noting, that I won't take into account the days prior to the given
month. That is, the constraints will be considered as per-month, not per
rolling 31 day period. Adding support for checking the last five days of the
previous month (to satisfy Considerations 3, 4 and 5) would not be difficult.

## Tools

I think I'll use Qt, since it has some very nice abstract data types, and I'm
quite familiar with it. Porting the logic to Ruby, Python, PHP etc would be
quite easy, and more an exercise in understanding the semantics of the ADTs in
those languages - which wouldn't take too much longer, but I'm a little time
constrained at the moment ;)
