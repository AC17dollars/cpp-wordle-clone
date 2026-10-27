// Sets the macOS Dock icon at runtime from embedded .ico bytes.
#import <AppKit/AppKit.h>
#include <cstddef>

void SetMacDockIcon(const unsigned char *data, size_t len)
{
	@autoreleasepool
	{
		NSImage *img = [[NSImage alloc] initWithData:[NSData dataWithBytes:data length:len]];
		if (img)
			[NSApp setApplicationIconImage:img];
	}
}
