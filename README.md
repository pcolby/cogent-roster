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
line, since it has no trailing newline character (a common *gotcha* with `wc`).

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

These duplicates could be valid, different nurses with the same name, or errors
in the data. In the former case, we would actually need unique IDs of some
kind, and in the latter, we need to ignore duplicates. Of course, in a real
world scenario, this would be a question for the customer using the product,
but since this is a coding test, we'll simply support both options. In the case
of needing unique IDs, we'll simply append line numbers to duplicate names to
differentiate them.

### How many Nurses does it take to Fill a Roster?

Its worth noting, that with so many nurses (98 if we exclude duplicates), we
can use a very naive least-recently-used (LRU) scheduling algorithm. This is
because:

* the maximum number of slots in a month is 465 (that is 31 days * 3 shifts * 5
  nurses per shift)
* a simple LRU algorithm would spread each nurse's shifts as far apart as
  possible
* each nurse would have only 4 or 5 shifts for this entire month (a mean of
  4.74 shifts per nurse per month)
* if 4 or 5 shifts is spread uniformly over the month, they can't possibly
  violate any of the stated *Considerations*

So it could be worth clarifying the situation with the customer - ie does the
customer guarantee that they always have so many if nurses available?  If so,
we can potentially save effort (and complexity) by using a very simple LRU
algorithm. However, again, since this is a coding exercise, let's assume that
such a guarantee is not available.

Even as we reduce the pool of available nurses, the same LRU algorithm would be
optimal, if it weren't for one *Consideration*: the need to group days off into
at least pairs.  Without this Consideration, spreading the available nurses as
efficiently as possible would work fine (for all given *Considerations*). It
should be pretty easy to overlay the at-least-two-dayas-off-at-a-time constraint
over an LRU algorithm though. Will come back to that later.

So, how many nurses do we need? (Not necessarily important to know for the
solution, but might help us to understand the problem space a little more).

It depends on which *Considerations* we apply.

We definitely need at least as many nurses as there are days in the month
(28, 29, 30 or 31). This is only a 1:1 correlation because the number of nurses
required per shift (5) is the same as the number of night shifts a nurse can
work per month (also 5).

The maths here is pretty simple:

```
number_of_nurses = ceil(night_slots_per_month / nights_per_month_per_nurse)
                 = ceil(nights_per_month * nurses_per_shift / nights_per_month_per_nurse)
                 = ceil( [29,31] * 5 / 5 )
```

This is the most restrictive *Considerations*. Indeed, if we exclude it, all
other *Considerations* can be met ad infinitum with just 21 nurses, or 18 if we
also skip the days-off-grouped-to-two-or-more (can explain the simple stagger
pattern required in more detail, but seems like overkill at this point).

All of this assumes that we're not allowing other, undocumented constraints,
such as nurses that can or can't work certain days, or wants to or can't work
with specific other nurses (or doctors). These features would be common in a
medical practice management system, but outside the scope of the current
solution.

Having said that, I'm leaning towards a simple algorithm that first applies
a constraints-based reduction of available nurses per slot, then selects from
the remaining nurses with a simple LRU scheduling heuristic. (I refer to
"scheduling" here in the [computing sense][1], not the business sense. By
separating the logic into constraints and scheduling, we can add further
constraints quite easily, and even swap out the scheduling algorithm (eg we
might want cheapest nurses, or nurses with the least number of complaints,
etc).

The disadvantage to this approach, is simply that if the number of constraints
became substantial (specifically the must / can't work specific dates types)
then the proposed algorithm may make early choices that prevent it being able
to satisfy later constraints. In this case, the algorithm would need to be to
begin with those temporal constraints and work backward to reach a solution in
a meaningful timeframe.

Also worth noting that I won't take into account the days prior to the given
month. That is, the constraints will be considered as per-month, not per
rolling 31 day period. Adding support for checking the last five days of the
previous month (to satisfy *Considerations* 3, 4 and 5) would not be difficult,
just extra as-yet-unjustified complexity.

### Tools

I think I'll use [Qt], since it has some very nice abstract data types, and I'm
quite familiar with it. Porting the logic to Ruby, Python, PHP etc would be
quite easy, and more an exercise in understanding the semantics of the ADTs in
those languages - which wouldn't take too much longer, but I'm a little time
constrained at the moment ;)

## The Solution

Attached is a simple, cross-platform [Qt]-based C++ application. That works
like this:

1. it reads the desired year and month from the command line (the year can be
   anything positive, well into the past or the future; the month has to be
   between 1 and 12)
2. it reads a list of nurses either from a named file or `stdin` (size is
   limited only by available RAM)
3. for each of the shifts in the given month, a `RosterGenerator` applies
   all of the registered constraint objects (these can be controlled from the
   command line) - these constraint objects restrict the list of nurses to only
   those that can be rostered for the given shift without violating the
   constraint.
4. once the generator has a list of viable shift candidates, it invokes a
   scheduler to choose the next nurse - currently, the only implemented
   scheduler is a least-recently-used scheduler, which is most "fair".
5. finally, the roster generator returns the shifts in a container, along
   with some metadata, which the main function outputs as a JSON document.

By using this approach, we can add/remove constraints quite easily (both the
current known constraints, and perhaps more importantly, any future constraints
that may arise). We can also plug-in new scheduling algorithms if so desired.

### Usage

The application outputs a basic usage message when given a `-h` argument (or
invalid arguments).

```
Usage: roster [options] YYYY MM
Generate nursing rosters

Options:
  -h, --help           Displays this help.
  -d, --debug          Enable debug output
  --no-color           Do not color the output
  --no-c1              Skip constraint 1 (AtMostFiveConsecutiveDays)
  --no-c2              Skip constraint 2 (AtMostFiveNightShiftsPerMonth)
  --no-c3              Skip constraint 3 (AtMostOneShiftPerDay)
  --no-c4              Skip constraint 4 (NoSingleDaysOff)
  -c, --compact        Use compact output
  -i, --nurses <file>  Read names of available nurses from file (default is
                       stdin)
  -o, --output <file>  Write output to file (default is stdout)
  --skip-dups          Skip duplicate nurse names

Arguments:
  YYYY MM              Month to produce roster for, in either ISO 8601 format
                       such as 'YYYY-MM'
```

So, for example, to generate a roster for June, using the supplied nurses list:

```
roster -i path/to/nurses.txt 2018 6
```

Which will output something like:
```json
{
    "2018-06": [
        {
            "evening": [
                "Gardenia",
                "Galla",
                "Garvey",
                "Galen",
                "Gal"
            ],
            ...
        }
    ],
    "created": "Fri. May 18 21:15:21 2018"
}
```

Here the `2018-06` property's value is an array of 30 days, each day including
three property - one for each shift, containing the five scheduled nurses for
each shift.

The `-h, --help`, `-d, --debug` and `--no-color` options should be pretty self-
explanatory.

The `--no-c1` to `--no-c4` options disable the respective constraints, allowing
the use of fewer nurses, if desired.

The `-c, --compact` argument simply makes the JSON output more compact - ie
using no superfluous whitespace, such as:

```
{"2018-06":[{"evening":["Galen","Ganiz","Garrison","Gemma","Gale (91)"],...}
```

The `-i, --nurses` and `-o, --output` options specify which files to read and
write respectively. They default to `stdin` and `stdout` if not specified.

Finally, the `--skip-dups` options tells the application to simply ignore any
duplicate names found in the list of nurses.  If not specified, any duplicates
will have ` (<line-number)` appended as many times as required to form a unique
name. For example, a second `Alice` found on line 42, might be read as
`Alice (42)`.

### Building

The application uses a cross-platform toolkit to support Windows, OSX, and
Linux. Out-of-source builds are fully supported (and recommended), so build
like:

```sh
mkdir -p /path/to/temporay/build/directory
cd /path/to/temporay/build/directory
qmake /path/to/source/checkout
make && make check
```

For Windows, replace `make` in the last line with either `nmake.exe` or
`mingw32-make.exe` accordingly.

#### Test Coverage

The project includes automated tests that cover all of the constraint,
scheduler, and generator code. The steps above will run the tests (ie the
`make check` at the end).

Test coverage reports can be generated (not currently available on Windows) by
setting the `ENABLE_COVERAGE` environment variable to `true`, and building the
`coverage` target after the tests have run, for example:

```sh
export ENABLE_COVERAGE=true
mkdir -p /path/to/temporay/build/directory
cd /path/to/temporay/build/directory
qmake /path/to/source/checkout
make && make check
make coverage
```

This will output a `coverage_html` directory containing the coverage report.
A copy of the latest generated report is included in the `doc/coverage`
directory at the root of this repository.

## What's Next

If this was a real project next steps would include validation of the solution,
such as:
* QA
* stakeholder demonstration
* user acceptance testing

However, the exact steps and processes would depend on the type of engagement,
level of product management involved, etc, and their direction or
prioritisation.

On the technical side, if this was a personal project, and in the absence of
other business imperatives, I would:
* configure CI/CD environments to build, and test, for all supported platforms
  automatically
* move some of the code from `main.cpp` to separate link units to enable
  additional tests
* automate (and validate) the generation of documentation from the code
* and probably lots of other things too, depending on the vision for the
  project

[1]: https://en.wikipedia.org/wiki/Scheduling_(computing) "Scheduling (computing)"
[Qt]: https://www.qt.io
