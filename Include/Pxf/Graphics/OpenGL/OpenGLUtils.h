#ifndef _PXF_GRAPHICS_OPENGLUTILS_H_
#define _PXF_GRAPHICS_OPENGLUTILS_H_

#include <Pxf/Graphics/OpenGL/OpenGL.h>

namespace Pxf 
{
	namespace Graphics 
	{
		// Should this be elsewhere?
		bool IsExtensionSupported( char* szTargetExtension )
		{
			const unsigned char *pszExtensions = NULL;
			const unsigned char *pszStart;
			unsigned char *pszWhere, *pszTerminator;

			// Extension names should not have spaces
			pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
			if( pszWhere || *szTargetExtension == '\0' )
				return false;

			// Get Extensions String
			pszExtensions = glGetString( GL_EXTENSIONS );

			// Search The Extensions String For An Exact Copy
			pszStart = pszExtensions;
			for(;;)
			{
				pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
				if( !pszWhere )
					break;
				pszTerminator = pszWhere + strlen( szTargetExtension );
				if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
					if( *pszTerminator == ' ' || *pszTerminator == '\0' )
						return true;
				pszStart = pszTerminator;
			}
			return false;
		}
	}
}

#endif // _PXF_GRAPHICS_OPENGLUTILS_H_
