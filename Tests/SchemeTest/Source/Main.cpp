#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <s7.h>

using Pxf::Util::String;

static s7_pointer our_exit(s7_scheme *sc, s7_pointer args) 
{
	/* all added functions have this form, args is a list, 
	*    s7_car(args) is the 1st arg, etc 
	*/
	exit(1);
	return(s7_nil(sc)); /* never executed, but makes the compiler happier */
}


bool PxfMain(String _CmdLine)
{
	s7_scheme *s7;
	char buffer[512];
	char response[1024];

	s7 = s7_init();                     /* initialize the interpreter */
	s7_define_function(s7, "exit", our_exit, 0, 0, false, "(exit) exits the program");
	/* add the function "exit" to the interpreter.
	*   0, 0, false -> no required args,
	*                  no optional args,
	*                  no "rest" arg
	*/
	while (1)                           /* fire up a REPL */
	{
		fprintf(stdout, "\n> ");        /* prompt for input */
		fgets(buffer, 512, stdin);
		if ((buffer[0] != '\n') || 
			(strlen(buffer) > 1))
		{                            
			sprintf(response, "(write %s)", buffer);
			s7_eval_c_string(s7, response); /* evaluate input and write the result */
		}
	}

	return true;
}