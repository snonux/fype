#*
 * Examples of how to use functions
 *#

fun foo {
	say 1 + a * 3 + b;

	fun bar {
		say "Hello i am nested";
	}

	bar; # Calling nested
}

my a = 2, b = 4; # Create global variables
foo;
assert 0 == (defined bar); # bar is not available anymore

fun baz {
	say "I am baz";
	undef baz; 
}

baz; # Baz deletes itself
assert 0 == (defined baz); # baz is not available anymore
