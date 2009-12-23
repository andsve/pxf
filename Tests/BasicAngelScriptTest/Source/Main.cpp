#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>

#ifndef CONF_WITH_ANGELSCRIPT
#error "Not compiled with angelscript support"
#endif

#include <scriptbuilder.h>
#include <angelscript.h>

using Pxf::Util::String;


// Implement a simple message callback function
void MessageCallback(const asSMessageInfo *msg, void *param)
{
	const char *type = "ERR ";
	if( msg->type == asMSGTYPE_WARNING ) 
		type = "WARN";
	else if( msg->type == asMSGTYPE_INFORMATION ) 
		type = "INFO";
	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}


// Print the script string to the standard output stream
void print()
{
	printf("hello");
}


bool PxfMain(String _CmdLine)
{
	int r;

	// Create the script engine
	asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	engine->SetEngineProperty(asEP_SCRIPT_SCANNER, 0);


	// Set the message callback to receive information on errors in human readable form.
	// It's recommended to do this right after the creation of the engine, because if
	// some registration fails the engine may send valuable information to the message
	// stream.
	r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL); assert( r >= 0 );

	// Register the function that we want the scripts to call 
	r = engine->RegisterGlobalFunction("void print()", asFUNCTION(print), asCALL_CDECL); assert( r >= 0 );


	// The CScriptBuilder helper is an add-on that loads the file,
	// performs a pre-processing pass if necessary, and then tells
	// the engine to build a script module.
	CScriptBuilder builder;
	r = builder.StartNewModule(engine, "MyModule");
	r = builder.AddSectionFromFile("test.as");
	r = builder.BuildModule();
	if( r < 0 )
	{
		// An error occurred. Instruct the script writer to fix the 
		// compilation errors that were listed in the output stream.
		printf("Please correct the errors in the script and try again.\n");
		return false;
	}

	// Find the function that is to be called. 
	asIScriptModule *mod = engine->GetModule("MyModule");
	int funcId = mod->GetFunctionIdByDecl("void main()");
	if( funcId < 0 )
	{
		// The function couldn't be found. Instruct the script writer
		// to include the expected function in the script.
		printf("The script must have the function 'void main()'. Please add it and try again.\n");
		return false;
	}

	// Create our context, prepare it, and then execute
	asIScriptContext *ctx = engine->CreateContext();
	ctx->Prepare(funcId);
	r = ctx->Execute();
	if( r != asEXECUTION_FINISHED )
	{
		// The execution didn't complete as expected. Determine what happened.
		if( r == asEXECUTION_EXCEPTION )
		{
			// An exception occurred, let the script writer know what happened so it can be corrected.
			printf("An exception '%s' occurred. Please correct the code and try again.\n", ctx->GetExceptionString());
		}
	}

	// Clean up
	ctx->Release();
	engine->Release();




	return true;
}