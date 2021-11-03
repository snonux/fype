NAME
    Fype is For Your Program Execution

    Fype is Free Yak Programed for ELF

    It's not a hype - it's Fype!

SYNOPSES
    fype [-[hsvTV]] file.fy
    fype -e "fype code string;"

ABOUT
    Fype is not yet released! Lots of basic stuff is still missing! See the
    TODO file of the source distribution of Fype!

    Fype is a 32 bit scripting language created for fun. You can use it and
    comment on it if you like. You could also write patches and mail them to
    fype at dev dot buetow dot org! Or go visit the IRC channel #coding at
    irc.german-elite.net and ask rantanplan.

    Fype is developed under the BSD license. See the COPYING file which is
    included in the source tree.

    Fype should be "at least as good as AWK". This does not mean it will be
    a replacement for AWK but it should support functions, loops and arrays
    like AWK does but with a different syntax. It should also get extension
    support like AWK has. I am not talking about GNU AWK but about New AWK,
    which is included in the *BSD distributions! Fype already has a few
    features which are not available in AWK. However, AWK has still many
    features which are not available in Fype as well ;)

    Fancy stuff like OOP or unicode or threading is not yet planed. But
    fancy stuff like function pointers and closures may be considered for
    one of the first releases of Fype :)

PARSING / CODE GENERATION
    The Fype syntax is very simple and is using a maximum look ahead of 1
    and a very easy top down parsing. Fype is parsing and interpreting its
    code simultaneous. This means, that syntax errors are only detected
    during program runtime.

REQUIREMENTS
    Fype only has been tested on FreeBSD 7.0. Linux may work too. Windows
    support is not planned. But may work using Cygwin or SFU. Linux users
    have to install pmake before compiling Fype!

    You will need:

            GNU GCC C Compiler (gcc.gnu.org)
            NetBSD Make aka pmake (GNU Make will not work)

    NetBSD Make is included in any *BSD and can be run with just make.

GETTING STARTED
    On Linux: Extract, compile and install Fype:

            tar xvjf fype.tar.bz2
            cd fype
            pmake
            sudo pmake install 
            pmake clean

    On FreeBSD: Extract, compile and install Fype:

            tar xvjf fype.tar.bz2
            cd fype
            make
            sudo make install 
            make clean

    Run a .fy file:

            fype test.fy

    See the ./examples subdir of the Fype source distribution for examples!
    See also fype -h for a list of all options.

DATA TYPES
    Fype uses auto type conversion. However, if you want to know what's
    going on you may take a look at the provided basic datatypes.

  The basic data types
    *integer*
        Specifies an integer number

    *double*
        Specifies a double number

    *string*
        Specifies a string

    *number*
        May be an integer or a double number

    *any*
        May be of any type above

    *void*
        No type

    *identifier*
        It's a variable name or a procedure name or a function name

  Explicit type conversions
    (*integer*) integer *any*
        Converts any type to an integer

    (*double*) double *any*
        Converts any type to a double

    (*string*) string *any*
        Converts any type to a string

SYNTAX
  Comments
    Text from a # character until the end of the current line is considered
    being a comment. Multi line comments may start with an #* and and with
    an *# anywhere. Exceptions are if those signs are inside of strings.

  Statements
    A Fype program is a list of statements. Each keyword, expression or
    function call is part of a statement. Each statement is ended with a
    semicolon. Example:

            my bar = 3, foo = 1 + 2; 
            say foo;
            exit foo - bar;

  Paranthesis
    All paranthesis of function calls are optional. They help to make the
    code better readable. They also help to force precedences of
    expressions.

  Scopeing
    A new scope starts with an { and ends with an }. An exception is a
    procedure, which does not use its own scope (see later in this manual).
    Control statements and functions support scopeings. The scope function
    will print out all available symbols at the current position. Here is a
    small example of how to use scopes:

            my foo = 1;

            {
                    # Prints out 1
                    put defined foo;
                    {
                            my bar = 2;

                            # Prints out 1
                            put defined bar;

                            # Prints out all available symbols at this
                            # point to stdout. Those are: bar and foo
                            scope;
                    }

                    # Prints out 0
                    put defined bar;

                    my baz = 3;
            }

            # Prints out 0
            say defined bar;

  Control statements
    Fype knows the following control statements:

    if *<expression*> { *<statements*> }
        Runs the statements if the expression evaluates to a true value.

    ifnot *<expression*> { *<statements*> }
        Runs the statements if the expression evaluates to a false value.

    while *<expression*> { *<statements*> }
        Runs the statements as long as the the expression evaluates to a
        true value.

    until *<expression*> { *<statements*> }
        Runs the statements as long as the the expression evaluates to a
        false value.

VARIABLES
    Variables can be defined with the my keyword. If you don't assign a
    value during declaration, then it's using the default integer value 0.
    Variables may be changed during program runtime. Variables may be
    deleted using the undef keyword! Example of defining variables:

            my foo = 1 + 2;
            say foo; 

            my bar = 12, baz = foo;
            say 1 + bar;
            say bar;

            my baz;
            say baz; # Will print out 0

    You may use the defined keyword to check if an identifier has been
    defined or not.

            ifnot defined foo {
                    say "No foo yet defined";
            }

            my foo = 1;

            if defined foo {
                    put "foo is defined and has the value ";
                    say foo;
            }

SYNONYMS
    Each variable can have as many synonyms as wished. A synonym is another
    name to access the content of a specific variable. Here is an example of
    how to use synomyms:

            my foo = "foo";
            my bar = \foo;
            foo = "bar";

            # The synonym variable should now also set to "bar"
            assert "bar" == bar;

    Synonyms can be used for all kind of identifiers. It's not limited to
    normal variables but can be also used for function and procedure names
    etc.

            # Create a new procedure baz
            proc baz { say "I am baz"; }

            # Make a synonym baz, and undefine baz
            my bay = \baz;

            undef baz;

            # bay still has a reference of the original procedure baz
            bay; # this prints aut "I am baz"

    The syms keyword gives you the total number of synonyms pointing to a
    specific value:

            my foo = 1;
            say syms foo; # Prints 1

            my baz = \foo; 
            say syms foo; # Prints 2
            say syms baz; # Prints 2

            undef baz;
            say syms foo; # Prints 1

BUILT IN FUNCTIONS
    In Fype, operators are built in functions as well. The difference is,
    that they may be written in infix notation instead in front of the
    arguments. The types inside the () specify the return types.

  Math
    (*any*) *any* + *any*
        Special string behavior: A string will get auto convertet into an
        *integer*.

    (*any*) *any* - *any*
        Special string behavior: A string will get auto convertet into an
        *integer*.

    (*any*) *any* * *any*
        Special string behavior: A string will get auto convertet into an
        *integer*.

    (*any*) *any* / *any*
        Special string behavior: A string will get auto convertet into an
        *integer*.

  Conditional
    (*integer*) *any* == *any*
    (*integer*) *any* != *any*
    (*integer*) *any* <= *any*
    (*integer*) *any* >= *any*
    (*integer*) *any* < *any*
    (*integer*) *any* > *any*
    (*integer*) not *any*

  Definedness
    (*integer*) defined *identifier*
        Returns 1 if *identifier* has been defined. Returns 0 else.

    (*integer*) undef *identifier*
        Tries to undefine/delete the *identifier*. Returns 1 if success,
        otherwise 0 is returned.

  Bitwise
    (*integer*) *any* :< *any*
    (*integer*) *any* :> *any*
    (*integer*) *any* and *any*
    (*integer*) *any* or *any*
    (*integer*) *any* xor *any*

  Numeric
    (*number*) neg *number*
        This function returns the negative value of *any*

    (*integer*) no [*integer*]
        This function returns 1 if the argument is 0, otherwise it will
        return 0! If no argument is given, then 0 is returned!

    (*integer*) yes [*integer*]
        This function always returns 1. The parameter is optional.

                # Prints out 1, because foo is not defined
                if yes { say no defined foo; }

  System
    (*void*) end
        Exits the program with the exit status of 0

    (*void*) exit *integer*
        Exits the program with the specified exit status

    (*integer*) fork
        Fork forks a subprocess. It returns 0 for the child process and the
        pid of the child process otherwise! Example:

                my pid = fork;

                if pid {
                        put "I am the parent process; child has the pid ";
                        say pid;

                } ifnot pid {
                        say "I am the child process";
                }

    (*integer*) gc
        Executes the garbage collector and returns the number of items
        freed! You may wonder why most of the time it will return a value of
        0! Fype tries to free not needed memory asap. This may change in
        future versions in order to gain faster execution of scripts!

  I/O
    (*any*) put *any*
        Prints out the argument

    (*any*) say *any*
        Same as put, but also includes an ending newline

    (*void*) ln
        Just prints a newline

SELF DEFINING PROCEDURES AND FUNCTIONS
  Procedures
    A procedure can be defined with the proc keyword and deleted with the
    undef keyword. A procedure does not return any value and does not
    support parameter passing. It's using already defined variables (e.g.
    global variables). A procedure does not have its own namespace. It's
    using the calling namespace. It is possible to define new variabes
    inside of a procedure in the current namespace.

            proc foo {
                    say 1 + a * 3 + b;
                    my c = 6;
            }

            my a = 2, b = 4;

            foo; # Run the procedure. Print out "11\n"
            say c; # Print out "6\n";

  Nested procedures
    It's possible to define procedures inside of procedures. Since
    procedures don't have its own scope, nested procedures will be available
    to the current scope as soon as the main procedure has run the first
    time. You may use the defined keyword in order to check if a procedure
    has been defined or not.

            proc foo {
                    say "I am foo";

                    undef bar;
                    proc bar {
                            say "I am bar";
                    }
            }

            # Here bar would produce an error because 
            # the proc is not yet defined!
            # bar; 

            foo; # Here the procedure foo will define the procedure bar!
            bar; # Now the procedure bar is defined!
            foo; # Here the procedure foo will redefine bar again!

  Functions
    A function should be defined with the func keyword and deleted with the
    undef keyword. Function not yet return values (will be changed in future
    versions) and supports not yet parameter passing (will be changed in
    future versions). It's using local (lexical scoped) variables. If a
    certain variable does not exist It's using already defined variables
    (e.g. one scope above).

            func foo {
                    say 1 + a * 3 + b;
                    my c = 6;
            }

            my a = 2, b = 4;

            foo; # Run the procedure. Print out "11\n"
            say c; # Will produce an error, because c is out of scoped!

  Nested functions
    Nested functions work the same way the nested procedures work, with the
    exception that nested functions will not be available any more after the
    function has been left!

            func foo {
                    func bar {
                            say "Hello i am nested";
                    }

                    bar; # Calling nested
            }

            foo;
            bar; # Will produce an error, because bar is out of scope!

AUTHOR
    Paul C. Buetow (http://paul.buetow.org)

WEBSITE
    The Fype Language (http://fype.buetow.org)

SEE ALSO
    awk(1) cc(1) make(1)

