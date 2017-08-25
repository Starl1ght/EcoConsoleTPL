Последняя, актуальная версия для C++ Siberia 2017 будет залита до первого сентября.

My best code written at time point of Mar 2017.

Challenge was to make little console, where we can add new commands as simple as possible and callbacks should be native (i.e. no vector of strings and manual convertation).

And well... It was a huge success. Console automatically checks for validity and count of supplied arguments, convert them to necessary types in callbacks and executes callback.

To add new command - simply write `MakeCommand("name", callback)`

To add new type for conversion - specialize `Converter` type
